/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _PROG_H_RPCGEN
#define _PROG_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct buffer {
	char *data;
};
typedef struct buffer buffer;

#define RLSPROG 0x20000008
#define RLSVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define RLS 1
extern  buffer * rls_1(buffer *, CLIENT *);
extern  buffer * rls_1_svc(buffer *, struct svc_req *);
extern int rlsprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define RLS 1
extern  buffer * rls_1();
extern  buffer * rls_1_svc();
extern int rlsprog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_buffer (XDR *, buffer*);

#else /* K&R C */
extern bool_t xdr_buffer ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_PROG_H_RPCGEN */
