$fn = 100;
translate([0,0,5])top();
//translate([0,0,2])holder();
 
 //translate([-9.3,-9.3,0])import("joycon.stl");
//translate([0,0,-10])handle(20);
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
                    translate([15, 8.5, 0]) circle(r = 1);
                    translate([8, 9, 0]) circle(r = 0.5);
                }
                hull() {
                    translate([15, 8.5, 0]) circle(r = 1);
                    translate([15, 0, 0]) circle(r = 1);
                }     
            } 
            
            // and now for the buttons
               translate([0,0,1])  
            difference(){
            hull(){
rotate([0,0,-5])translate([0,-14,-2])sphere(d=12);
rotate([0,0,40])translate([0,-14,-2])sphere(d=12);
      
rotate([0,0,-5])translate([0,-14,-12])rotate([17,0,0])translate([0,0,15])cylinder(d1=22,d2=22,h=4);
rotate([0,0,40])translate([0,-14,-12])rotate([17,0,0])translate([0,0,15])cylinder(d1=22,d2=22,h=4);      
      }
      cylinder(d=28,h=20);
  }
        }
        
        translate([-20,-40,-10])cube([50,60,12]);
        
        
        translate([0,0,1]){
              rotate([0,0,-5])translate([-6.5,-26.5,2])rotate([17,0,0])cube([13,12,5]);
  rotate([0,0,40])translate([-6.5,-26.5,2])rotate([17,0,0])cube([13,12,5]);
         
                       rotate([0,0,-5])translate([-6.5,-24,1])rotate([17,0,0])cube([13,7.5,5]);
            
  rotate([0,0,40])translate([-6.5,-24,1])rotate([17,0,0])cube([13,7.5,5]);   
            
            
  rotate([0,0,-5])translate([-3,-22,3])cube([6,22,3]);
  rotate([0,0,40])translate([-3,-22,3])cube([6,22,3]);
        } 
        
        // and the lower edges
      cylinder(d=28.4,h=4.5);//better fit  
      translate([-20,-20,0])cube([40,40,1.8]);  
    }

}

module holder(){
difference() {
    union(){
    translate([0,0,-3])cylinder(d = 32, h = 1);
    translate([0,0,-5])cylinder(d=28,h=5.5);  
  }
  translate([-20, 0, -15])rotate([-10, 0, 0])
  union() {
    rotate([0, 90, 0])cylinder(d = 20, h = 40);
    translate([0, -10, -60])cube([40, 20, 60]);
  }
  //translate([-7,-7,-4])potmeters();
  translate([0,0,1])cylinder(d=26,h=2);
  translate([-9.3,-9.3,-1])import("joycon.stl");
//  translate([-9,-9,-1])import("joycon.stl");
//  translate([-9,-9,-2])import("joycon.stl");
  //translate([-6.8,-10.8,-2])cylinder(d=3.5,h=5);
  
  translate([-3,11.5,-5])cube([6,2.5,10]); 
 translate([-3,-12,-4])cube([6,24,10]); 
  
  translate([-8.4,-8.2,-4])cube([10.5,17,4]);
  translate([-9.6,-9.6,-4])cube([4,4,10]);
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
     rotate([-angle, 0, 0])translate([-4,3,-32])rotate([0,90,0])doughnut(12,2,8);
    }

    
    translate([0, 0, -13])rotate([-angle, 0, 0])
    fingercutout(45,20);
    
    translate([0, 0, 0])cylinder(d = 28.8, h = 10);
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
    translate([0,0,-height])cylinder(d=20,h=height);
    translate([0,0,0])sphere(d=20);
    translate([0,0,-height])sphere(d=20);
    rotate([0,0,angle/2])translate([-40,0,0])
    union() {
      rotate([0, 90, 0])cylinder(d = 20, h = 40);
       translate([0,0,-height])rotate([0, 90, 0])cylinder(d = 20, h = 40);
       translate([0,-10,-height])cube([40,20,height]); 
    }

    rotate([0,0,-angle/2])translate([0,0,0])
        union() {
      rotate([0, 90, 0])cylinder(d = 20, h = 40);
      translate([0,0,-height])rotate([0, 90, 0])cylinder(d = 20, h = 40);
                  translate([0,-10,-height])cube([40,20,height]);
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