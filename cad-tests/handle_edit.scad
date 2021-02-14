linear_extrude(height=20, center=true) {
    import("fingercutout_contour.dxf");
}
mirror([0,1,0])
translate([0,0,10])
rotate([90,0,0])
rotate_extrude(angle=45) {
    import("fingercutout_contour.dxf");
}