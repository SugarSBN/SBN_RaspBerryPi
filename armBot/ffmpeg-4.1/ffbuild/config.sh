# Automatically generated by configure - do not modify!
shared=yes
build_suffix=
prefix=/opt/ffmpeg
libdir=${prefix}/lib
incdir=${prefix}/include
rpath=
source_path=.
LIBPREF=lib
LIBSUF=.a
extralibs_avutil="-pthread -lm"
extralibs_avcodec="-pthread -lm -llzma -lz -lx264"
extralibs_avformat="-lm -lz"
extralibs_avdevice="-lm -lxcb -lxcb-shm"
extralibs_avfilter="-pthread -lm"
extralibs_avresample="-lm"
extralibs_postproc="-lm"
extralibs_swscale="-lm"
extralibs_swresample="-lm"
avdevice_deps="avfilter swscale postproc avformat avcodec swresample avresample avutil"
avfilter_deps="swscale postproc avformat avcodec swresample avresample avutil"
swscale_deps="avutil"
postproc_deps="avutil"
avformat_deps="avcodec swresample avutil"
avcodec_deps="swresample avutil"
swresample_deps="avutil"
avresample_deps="avutil"
avutil_deps=""
