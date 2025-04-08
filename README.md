# Simple Memory Management Simulation

This project implements a simplified simulation of a [memory management system](https://en.wikipedia.org/wiki/Memory_management_unit) in C. It handles [virtual-to-physical address translation](https://en.wikipedia.org/wiki/Virtual_memory) and [page swapping between primary and secondary storage](https://en.wikipedia.org/wiki/Memory_paging).

This project was developed as part of the module "[Digitale Systeme](https://agnes.hu-berlin.de/lupo/rds?state=verpublish&status=init&vmfile=no&publishid=201319&moduleCall=webInfo&publishConfFile=webInfo&publishSubDir=veranstaltung)" (_digital systems_) in my Computer Science bachelor's program.

## Compilation and Execution

1. **Compilation:**  
   ```bash
   gcc -o soft_mmu main.c mmu.c
   ```

2. **Execution:**
   ```bash
   ./soft_mmu
   ```