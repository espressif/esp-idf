#ifndef _UNISTD_H_
#define _UNISTD_H_

# include <sys/unistd.h>

#ifndef L_SET
/* Old BSD names for the same constants; just for compatibility.  */
#define		L_SET		SEEK_SET
#define		L_INCR		SEEK_CUR
#define		L_XTND		SEEK_END
#endif

#endif /* _UNISTD_H_ */
