# Color benchmark Example

This is an example to test the performance of DALI for simple Toolkit::Control with background color.
Create 1 rootView and `R`x`C` views with random background colors. And animate the rootView's Opacity from
0.8f to 0.2f, So we could avoid optimazation case of full-opaque or full-transparent cases.
Application will be quit after `D` milliseconds.

Each values `R`, `C`, and `D` could be change by commandline
See below usages.

```shell
$ ./benchmark-color.example // R = 50, C = 20, D = 60000 (== 1 minute)
$ ./benchmark-color.example -r200 -c80 -d1000 // R = 200, C = 80, D = 1000 (== 1 seconds)
```




