#!/bin/sh
patch --dry-run -p1 < PATCHES_BEST/hwe/BFQ.patch
patch --dry-run -p1 < PATCHES_BEST/hwe/BFS.patch 
patch --dry-run -p1 < PATCHES_BEST/hwe/MISC-GOOD-MODED-4-hwe-patch.patch
