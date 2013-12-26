httpcameraserver
================
simple web-camera http server program.

usage
================

simple usage
--------
start httpcameraserver.exe.
<pre>
  C:\httpcameraserver\bin> httpcameraserver.exe
  start httpcameraserver. listen port=10080
</pre>

access http://localhost:10080/ using the web browser.

<pre>
start http://localhost:10080/
</pre>

sample client sketch for processing.
<pre>
  void setup() {
    size(640, 480);
  }
  
  void draw() {
    PImage img = loadImage("http://localhost:10080/camera.jpg");
    if (img != null) {
      image(img, 0, 0, width, height);
    }
  }
</pre>

commanline options
--------
<pre>
  C:\httpcameraserver\bin> httpcameraserver.exe -h
  options:
    -h [ --help ]                   print help
    -p [ --port ] arg (=10080)      listen port
    -i [ --camera_idx ] arg (=0)    open camera idx
    -j [ --jpeg_quality ] arg (=90) jpeg quality (1-100)
    -n [ --no_display ] arg (=0)    no display window mode...
</pre>
