```
#include "MemStats.h"

MemStats::printMemStats();


```

and you get mem stats on stdout

```
== HEAP STATS =========================================================
BaseAppDebug(59693,0x7fff7cdd5310) malloc: 
Bytes > total: 325MB, used: 138MB, free: 187MB
Chunks > used: 2714042 free: 0
=======================================================================
```

#Notes

Probably osx / linux only; only tested on OSX.
