#!/bin/bash -x
# Copyright 2014 Canonical Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# To call this script, make sure make_ext4fs is somewhere in PATH

function usage() {
cat<<EOT
Usage:
mkubuntuimg.sh [-s] [-i <inodes>] SRC_DIR OUTPUT_FILE EXT_VARIANT MOUNT_POINT SIZE [FS_CONTENT]
EOT
}

echo "in mkubuntuimg.sh PATH=$PATH"
ENABLE_SPARSE_IMAGE=
if [ "$1" = "-s" ]; then
  ENABLE_SPARSE_IMAGE="-s"
  shift
fi

INODES=
if [ "$1" = "-i" ]; then
  INODES="-i $2"
  shift 2
fi

if [ $# -ne 5 -a $# -ne 6 ]; then
  usage
  exit 1
fi

SRC_DIR=$1
if [ ! -d $SRC_DIR ]; then
  echo "Can not find directory $SRC_DIR!"
  exit 2
fi

OUTPUT_FILE=$2
EXT_VARIANT=$3
MOUNT_POINT=$4
SIZE=$5
CONTENT=$6
LABEL=""

case $EXT_VARIANT in
  ext4) ;;
  *) echo "Only ext4 is supported!"; exit 3 ;;
esac

if [ -z $MOUNT_POINT ]; then
  echo "Mount point is required"
  exit 2
fi

case $MOUNT_POINT in
  "/")
    LABEL="-L rootfs"
    ;;
  "custom")
    LABEL="-L custom"
    ;;
esac

if [ -z $SIZE ]; then
  echo "Need size of filesystem"
  exit 2
fi

if [ -f $CONTENT ]; then
    FS_CONTENT="-U $CONTENT"
fi
MAKE_EXT4FS_CMD="make_ext4fs $ENABLE_SPARSE_IMAGE $INODES -l $SIZE $LABEL -u $MOUNT_POINT $FS_CONTENT $OUTPUT_FILE $SRC_DIR"
echo $MAKE_EXT4FS_CMD
$MAKE_EXT4FS_CMD
if [ $? -ne 0 ]; then
  exit 4
fi
