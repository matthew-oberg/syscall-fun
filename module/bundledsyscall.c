#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/uio.h>

#include <netinet/in.h>

#define BUNDLE_ADDR "127.0.0.1"
#define BUNDLE_PORT 1234
#define BUNDLE_SZ   512

static struct thread* bundle_td;
static int bundle_state;
static struct socket* bundle_sock;
static struct sockaddr_in bundle_addr;
static struct mbuf* tx_buf;
static struct vnode* bundle_vn;

static int bundled_sock_init(void)
{
    int r;

    r = socreate(AF_INET, &bundle_sock, SOCK_DGRAM, IPPROTO_UDP, bundle_td->td_ucred, bundle_td);
    if (r != 0)
    {
        uprintf("Failed to create socket.\n");
        return r;
    }

    bundle_addr.sin_family = AF_INET;
    bundle_addr.sin_port = htons(BUNDLE_PORT);

    r = inet_pton(AF_INET, BUNDLE_ADDR, &bundle_addr.sin_addr);
    if (r < 1)
    {
        uprintf("Failed to interpret address.\n");
        return 1;
    }

    r = sobind(bundle_sock, (struct sockaddr*)&bundle_addr, bundle_td);
    if (r < 0)
    {
        uprintf("Failed to bind.\n");
        return 1;
    }

    tx_buf = m_get2(BUNDLE_SZ, M_NOWAIT, MT_DATA, M_PKTHDR);
    if (!tx_buf)
    {
        uprintf("Failed to create mbuf.\n");
        return 1;
    }

    return 0;
}

static int bundled_file_init(int fd)
{
    int r;

    r = fgetvp_read(bundle_td, fd, &cap_pread_rights, &bundle_vn);
    if (r != 0)
    {
        uprintf("Failed to read vnode.\n");
        return 1;
    }

    return 0;
}

static int bundledsyscall(struct thread* thrd, void* args)
{
    int r;
    int fd;
    static char rx_buf[BUNDLE_SZ];

    fd = *(int*)args;

    if (bundle_state == 0)
    {
        uprintf("Setting up bundled syscall.\n");
        bundle_state = 1;

        bundle_td = thrd;

        if (bundled_sock_init() != 0 && bundled_file_init(fd) != 0)
        {
            return -1;
        }
    }

    /*r = vn_rdwr(UIO_READ, bundle_vn, rx_buf, BUNDLE_SZ - 1, 0, UIO_SYSSPACE, IO_NODELOCKED, thrd->td_ucred, thrd->td_ucred, NULL, thrd);
    if (r != 0)
    {
        return -1;
    }*/

    m_copyback(tx_buf, 0, BUNDLE_SZ - 1, rx_buf);
    r = sosend_dgram(bundle_sock, (struct sockaddr*)&bundle_addr, NULL, tx_buf, NULL, 0, thrd);
    if (r < (BUNDLE_SZ - 1))
    {
        return 1;
    }

    return 0;
}

static struct sysent bundledsyscall_sysent = {
    .sy_narg = 1,
    .sy_call = bundledsyscall
};

static int offset = NO_SYSCALL;

static int load_bundledsyscall(struct module* module, int cmd, void* args)
{
    int r;

    r = 0;
    switch (cmd)
    {
        case MOD_LOAD:
            bundle_state = 0;
            uprintf("bundledsyscall loaded %d.\n", offset);
            break;
        case MOD_UNLOAD:
            uprintf("bundledsyscall unloaded.\n");
            break;
        default:
            r = EOPNOTSUPP;
            break;
    }

    return r;
}

SYSCALL_MODULE(syscall, &offset, &bundledsyscall_sysent, load_bundledsyscall, NULL);

