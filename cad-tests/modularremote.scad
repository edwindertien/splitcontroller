$fn = 100;



diameter = 18;
translate([0,0,4])top();
translate([0,0,2])holder();
translate([0,0,-6])beugel(20);
//wedge(60);  // between 10 and 60

//cup(0,0);
module ring(){
  difference() {
        union(){
            translate([0,0,2])cylinder(d = 32, h = 2);
            cheese(15,1,2);
        }
         translate([0, 0, 2])cylinder(d = 28.8, h = 10);
            translate([-3, 11.5, -2])cube([6, 5, 10]);
        
        }
         

         translate([-3,0,-11])rotate([90,0,90])doughnut(11,1.5,6);
    }      

module top() {
    difference(){
        union(){
            rotate_extrude() {
                hull() {
                    translate([15, 17.5, 0]) circle(r = 1);
                    translate([10, 18, 0]) circle(r = 0.5);
                }
                hull() {
                    translate([15, 17.5, 0]) circle(r = 1);
                    translate([15, 0, 0]) circle(r = 1);
                }     
            } 
               translate([0,0,8])  hull(){
rotate([0,0,-5])translate([0,-14,-2])sphere(d=12);
rotate([0,0,40])translate([0,-14,-2])sphere(d=12);
      
rotate([0,0,-5])translate([0,-14,-12])rotate([17,0,0])translate([0,0,15])cylinder(d1=22,d2=22,h=4);
rotate([0,0,40])translate([0,-14,-12])rotate([17,0,0])translate([0,0,15])cylinder(d1=22,d2=22,h=4);      
      }
        }
        translate([-20,-20,-5])cube([40,40,5]);
        cylinder(d=28,h=17.5);
        
        translate([0,0,8]){
              rotate([0,0,-5])translate([-6.5,-26.5,2])rotate([17,0,0])cube([13,12,5]);
  rotate([0,0,40])translate([-6.5,-26.5,2])rotate([17,0,0])cube([13,12,5]);
         
                       rotate([0,0,-5])translate([-6.5,-24,1])rotate([17,0,0])cube([13,7.5,5]);
            
  rotate([0,0,40])translate([-6.5,-24,1])rotate([17,0,0])cube([13,7.5,5]);   
            
            
  rotate([0,0,-5])translate([-3,-22,-4])cube([6,22,10]);
  rotate([0,0,40])translate([-3,-22,-4])cube([6,22,10]);
        }
      cylinder(d=28.4,h=4.5);//better fit  
      translate([-20,-20,0])cube([40,40,1.2]);  
    }

}
module wedge(angle){
difference(){
union(){
cylinder(d=28,h=3);
hull(){
translate([0,0,2])cylinder(d=32,h=3);
translate([angle/20,0,8+(angle-20)/3])rotate([0,angle,0])cylinder(d=32,h=2.5);
}
}
translate([angle/20,0,8+(angle-20)/3])rotate([0,angle,0])cylinder(d=28,h=3.1);
translate([angle/20,0,8+(angle-20)/3])rotate([0,angle,0])translate([-3,10,0])cube([6,30,5]);
}
}



module holder(){
difference() {
    union(){
    translate([0,0,-3])cylinder(d = 32, h = 1);
    translate([0,0,-5])cylinder(d=28,h=5);  
  }
  translate([-20, 0, -15])rotate([-10, 0, 0])
  union() {
    rotate([0, 90, 0])cylinder(d = 20, h = 40);
    translate([0, -10, -60])cube([40, 20, 60]);
  }
  translate([-7,-7,-4])potmeters();
  translate([0,0,1])cylinder(d=26,h=2);
//   rotate([0,0,-5])translate([-3,-22,-3])cube([6,22,10]);
//  rotate([0,0,40])translate([-3,-22,-3])cube([6,22,10]);
  //translate([0,-20,-50])rotate([-10,0,0])cylinder(d=5,h=70);
  translate([-3,11.5,-5])cube([6,2.5,10]); 
 translate([-3,-12,-4])cube([6,24,10]); 
}
}
//beugel(20);
module beugel(angle) {
    
  difference() {
    union() {

      hull() {
        cylinder(d = 32, h = 2);
        translate([0, -15, -22])rotate([90-angle, 0, 0])cheese(r1 = 10 - 2.5, r2 = 3, h = 4);
                  translate([0, 8, -22])rotate([90-angle, 0, 0])cheese(r1 = 10 - 2.5, r2 = 3, h = 4);
      }
     rotate([-angle, 0, 0])translate([-4,3,-32])rotate([0,90,0])doughnut(diameter/2+2,2,8);
    }

    
    translate([0, 0, -13])rotate([-angle, 0, 0])
    fingercutout(45,20);
    
    translate([0, 0, -0.3])cylinder(d = 28.8, h = 10);
    translate([-3, 11.5, -2])cube([6, 5, 10]);
  }
}


module handle(angle) {
    
  difference() {
    union() {

      hull() {
        cylinder(d = 32, h = 2);
        translate([0, -15, -22])rotate([90-angle, 0, 0])cheese(r1 = 10 - 2.5, r2 = 3, h = 4);
                  translate([0, 8, -22])rotate([90-angle, 0, 0])cheese(r1 = 10 - 2.5, r2 = 3, h = 4);
      }

    }

    
    translate([0, 0, -13])rotate([-angle, 0, 0])
    fingercutout(45,20);
    
    translate([0, 0, 0])cylinder(d = 28.8, h = 10);
    translate([-3, 11.5, -2])cube([6, 2.5, 10]);
    translate([-3, 11.5, -2])cube([6, 5, 10]);
  }
}



module fingercutout(angle,height){
    
    translate([0,0,-height])cylinder(d=diameter,h=height);
    translate([0,0,0])sphere(d=diameter);
    translate([0,0,-height])sphere(d=diameter);
    rotate([0,0,angle/2])translate([-40,0,0])
    union() {
      rotate([0, 90, 0])cylinder(d = diameter, h = 40);
       translate([0,0,-height])rotate([0, 90, 0])cylinder(d = diameter, h = 40);
       translate([0,-diameter/2,-height])cube([40,diameter,height]); 
    }

    rotate([0,0,-angle/2])translate([0,0,0])
        union() {
      rotate([0, 90, 0])cylinder(d = diameter, h = 40);
      translate([0,0,-height])rotate([0, 90, 0])cylinder(d = diameter, h = 40);
                  translate([0,-diameter/2,-height])cube([40,diameter,height]);
    }
}    

module originalhandle() {
  difference() {
    union() {

      hull() {
        cylinder(d = 32, h = 2);
        translate([0, -15, -35])rotate([80, 0, 0])cheese(r1 = 10 - 2.5, r2 = 3, h = 4);
      }

    }
    translate([0, 0, 0])cylinder(d = 28.8, h = 10);
    translate([-20, 0, -11])rotate([-10, 0, 0])

    union() {
      rotate([0, 90, 0])cylinder(d = 20, h = 40);
      translate([0, -10, -60])cube([40, 20, 60]);
    }
    translate([-3, 11.5, -5])cube([6, 2.5, 10]);
  }
}

//translate([-40,0,0])cylinder(d=24,h=8);
module cup(x,y){translate([0, 0, 5])rotate([x,y, 0]) {
  translate([0, 0, 5]) {
    translate([0, 0, 14]) {
      difference() {
        cheese(6.5, 2.5, 0);
        translate([0, 0, 26])sphere(d = 50);
      }
    }
    translate([0, 0, 5])cylinder(d = 9, h = 8);
    translate([0, 0, -7])difference() {
      sphere(d = 28);
      translate([-15, -15, -15])cube([30, 30, 22]);
    }
  }
}
}



module potmeters(){
translate([-3,2,0])cube([23,10,14]);

translate([2,1,0])cube([10,16,14]);

cube([14,14,14]);

}




module doughnut(r1, r2, h) {
  rotate_extrude() {
    hull() {
      translate([r1, h, 0])circle(r = r2);
      translate([r1, 0, 0])circle(r = r2);
    }
  }
}

module cheese(r1, r2, h) {
  hull() {
    rotate_extrude() {
      translate([r1, h, 0])circle(r = r2);
      translate([r1, 0, 0])circle(r = r2);
    }
  }
}