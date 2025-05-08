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

## Appendix

If you want to test with additional option, add options after `-p`.
We can add multiple optons.

`C` : Use `DevelControl::Property::CORNER_RADIUS`
`S` : Use `DevelControl::Property::CORNER_SQUARENESS`
`B` : Use `DevelControl::Property::BORDERLINE_WIDTH`
`b` : Use `DevelVisual::Property::BORDERLINE_WIDTH`
`H` : Use `DevelControl::Property::SHADOW`
`I` : Use `DevelControl::Property::INNER_SHADOW`
`O` : Use `DevelControl::Property::OFFSCREEN_RENDERING`
`E` : Use `DevelActor::Property::CHILDREN_DEPTH_INDEX_POLICY`

```shell
$ ./benchmark-color.example -pC /// Rounded Corner
$ ./benchmark-color.example -pCS /// Squircle Corner
$ ./benchmark-color.example -r20 -c20 -pOCB /// R = 20, C = 20 with OffscreenRendering::REFRESH_ALWAYS and Rounded Border
```




