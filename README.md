## Native Channel Change Test Application

GStreamer based multiple stream change test application for stability and stress.

### Configuration options

Use `-DCYCLE_URLS=1` to enable cycling through the test streams. 

Use `-DPLAY_DURATION_SEC=<seconds>` to configure the playback duration of a stream.

### Yocto recipe integration   

<details>
  <summary>Click to see sample recipe file</summary>  
  
```
DESCRIPTION = "GStreamer channel change test application."
SECTION = "Apps"
LICENSE = "Apache 2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=726a766df559f36316aa5261724ee8cd"

DEPENDS = "gstreamer1.0"

SRCREV = "${AUTOREV}"
SRC_URI = "git://github.com/arun-madhavan-013/gstreamer-channelchange-testapp.git;protocol=https;branch=master"

S = "${WORKDIR}/git"
inherit pkgconfig cmake
```

</details>  
