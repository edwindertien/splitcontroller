/* change $fn to 100 for better quality rendering
ONLY do this for final version, because minkowski gives your computer
a hard time, and a long wait for you.
*/
$fn = 50;
fragsstd = 100;
fragsminkw = 50;

fingercutoutdeg = 45;
fingercutouthght = 20;

diameter = 18;
translate([0,0,4])top();
translate([0,0,2])holder();
translate([0,0,-12])beugel(20);
//wedge(60);  // between 10 and 60
   


module top() {
       //thirdbuttoncontainer();
    difference(){
        union($fn=fragsstd){
            color( "Lime", 1.0 )
            rotate_extrude( $fn=50) {
                hull() {
                    translate([15, 17.5, 0]) circle(r = 1, $fn=fragsstd );
                    translate([10, 18, 0]) circle(r = 0.5, $fn=fragsstd );
                }
                hull() {
                    translate([15, 17.5, 0]) circle(r = 1, $fn=fragsstd );
                    translate([15, 0, 0]) circle(r = 1, $fn=fragsstd );
                }     
            } 
               translate([0,0,8])  
            color( "Red", 1.0 )hull($fn=fragsstd ){
rotate([0,0,-5])translate([0,-14,-2])sphere(d=12);
rotate([0,0,40])translate([0,-14,-2])sphere(d=12);
      
rotate([0,0,-5])translate([0,-14,-12])rotate([17,0,0])translate([0,0,15])cylinder(d1=22,d2=22,h=4, $fn=fragsstd );
rotate([0,0,40])translate([0,-14,-12])rotate([17,0,0])translate([0,0,15])cylinder(d1=22,d2=22,h=4, $fn=fragsstd );      
      }
      thirdbuttonelement(); 



        }
        translate([-20,-20,-5])cube([40,40,5]);
        // upper inner cylinder
         color( "RosyBrown", 1.0 ) cylinder(d=28,h=17.5);
        
        // third button container
        
        thirdbuttoncontainer();
        
        // button containers
           translate([0,0,8]){
             color( "RosyBrown", 1.0 ) rotate([0,0,-5])translate([-6.5,-26.5,2])rotate([17,0,0])cube([13,12,5]);
  color( "RosyBrown", 1.0 )rotate([0,0,40])translate([-6.5,-26.5,2])rotate([17,0,0])cube([13,12,5]);
         
                       rotate([0,0,-5])translate([-6.5,-24,1])rotate([17,0,0])cube([13,7.5,5]);
            
  rotate([0,0,40])translate([-6.5,-24,1])rotate([17,0,0])cube([13,7.5,5]);   
            
            
  color( "Tan", 1.0 )rotate([0,0,-5])translate([-3,-22,-4])cube([6,22,10]);
 color( "Tan", 1.0 ) rotate([0,0,40])translate([-3,-22,-4])cube([6,22,10]);
        }
        // bottom inner cylinder
       color( "Teal", 1.0 ) cylinder(d=28.4,h=4.5);//better fit  
        // flattened bottom
       color( "Tan", 1.0 ) translate([-20,-20,0])cube([40,40,1.2]);  
    }
    
    module thirdbuttoncontainer() {
          color( "OliveDrab", 0.5 )rotate([90,0,20])translate([-7,3,-22])
        union() {
        cube([13,12,7]);
           color( "DarkOrange", 1.0 )translate([0,2,-1]) cube([13,7.5,10]);
        }
    }
    
    module thirdbuttonelement() {
              color( "PaleVioletRed", 1.0 )rotate([0,0,80])translate([0,0,-11])
        minkowski($fn = fragsminkw)
        {
          rotate_extrude(angle = 60, convexity = 2)translate([16,20,-15])square([1.5,15],center=true);
          cylinder(r=2,h=1.5);
        }
        
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
    translate([0,0,-3])cylinder(d = 32, h = 1, $fn=fragsstd );
    translate([0,0,-5])cylinder(d=28,h=5, $fn=fragsstd );  
  }
  translate([-20, 0, -15])rotate([-10, 0, 0])
  union() {
    rotate([0, 90, 0])cylinder(d = 20, h = 40,  $fn=fragsstd );
    translate([0, -10, -60])cube([40, 20, 60]);
  }
  translate([-7,-7,-4])potmeters();
  translate([0,0,1])cylinder(d=26,h=2);
  translate([-3,11.5,-5])cube([6,2.5,10]); 
 translate([-3,-12,-4])cube([6,24,10]); 
}
}

module beugel(angle) {
    szbeugel = diameter + 5;
    szbeugelhght = fingercutouthght*2-5 ;
    hght =  szbeugelhght ;
color( "Coral", 1.0 )toppiece();
        
    translate([0,0,-7])
        minkowski()
        {

            
            difference() {
                rotate([-angle,0,0])
                union() {
                    
                    translate([0,0,-szbeugelhght/2])cube(size = [szbeugel,szbeugel,szbeugelhght], center = true);
                    translate([-szbeugel/2,0,-hght])rotate([0,90,0])cylinder(d = szbeugel, h = szbeugel);
                }
                translate([0, 0, -13])rotate([-angle, 0, 0])
                fingercutout(45,fingercutouthght);
            }
          cylinder(r=2,h=1);
        }
    
        
        module toppiece() {
            difference() {
            hull() {
                translate([0,0,2])cylinder(d = 32, h = 1);
                minkowski($fn = fragsminkw) {
                    rotate([-angle,0,0])translate([0,2.5,-6])cube(size = [szbeugel,szbeugel,2], center = true);
                    cylinder(r=2,h=1, $fn = fragsstd);
                }
            }
            // cuts
                translate([0, 0, -0.3])color( "PaleVioletRed", 1.0 )cylinder(d = 28.8, h = 10, $fn = fragsstd);
        translate([-3, 11.5, -2])color( "PaleVioletRed", 1.0 )cube([6, 5, 10]);
            
        }
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


module fingercutout(angl,height){
    radius = diameter /2;
    translate([0,-22,-height])
    rotate([0,0,67])element();
    module element() {
    translate([20,0,0])rotate([90,0,0])linear_extrude(height = 40)section();
    rotate_extrude(angle = 45,convexity = 10)translate([20,0,0])section();
    rotate([90,0,45])translate([20,0,-40])linear_extrude(height = 40)section();
       module section() {
         hull() {
          translate([0, height, 0])circle(r = radius);
          translate([0, 0, 0])circle(r = radius);
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