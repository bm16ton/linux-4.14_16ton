All patches need libst205 to be installed before compiling.

** mplayer-1.0rc2-libst2205.patch **
This is a patch against MPlayer v1.0rc2 to add support for a hacked ST2205
device as a video output device.

Ultra-short howto:
- Make sure libst2205 is installed
- Get & unpack MPlayer (www.mplayerhq.hu)
- Apply patch
- ./configure && ./make
- if you want, sudo make install
- mplayer -vo st2205u:/dev/sdX -x 128 -y 128 -zoom movie.avi
  (with /dev/sdX being the device your photo frame is connected to)


** lcd4linux **
included in latest svn release and upcomming v0.11.0 release
