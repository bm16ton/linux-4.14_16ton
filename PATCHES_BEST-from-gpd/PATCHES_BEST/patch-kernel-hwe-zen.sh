#!/bin/sh
patch -p1 < PATCHES_BEST/hwe/BFQ.patch
patch -p1 < PATCHES_BEST/hwe/BFS.patch 
patch -p1 < PATCHES_BEST/hwe/MISC-GOOD-MODED-4-hwe-patch.patch
patch -p1 < PATCHES_BEST/hwe/MUQSS.patch
patch -p1 < PATCHES_BEST/hwe/ZEN_INTERACTIVE.patch
