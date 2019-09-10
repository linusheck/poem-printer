PRINTER_WIDTH = 10.4;
PRINTER_DEPTH = 5.8;

BOX_WIDTH = 11.2;
BOX_DEPTH = 11.2;
BOX_HEIGHT = 7.2;

BUTTON_DIAMETER = 1.9;

WOOD_WIDTH = .4;

// To export:

// Comment this out
3D();

// Comment this in
// projection() {
//     2d();
// }

module Pins(length, wood_width, beginning, end) {
    for (i = [beginning : 4 : (length) / wood_width + end]) {
        translate([i * wood_width, 0, 0]) {
            cube([wood_width * 2, wood_width + .0001, wood_width + .0001]);
        }
    }
}

module Wall(width, height, wood_width, corners, beginnings, ends) {
    cube([width, height, wood_width]);
    translate([0, -wood_width, 0]) {
        Pins(width, wood_width, beginnings[0], ends[0]);
        if (corners) {
            translate([-wood_width, 0, 0]) 
            cube([wood_width, wood_width, wood_width]);
        }
    }
    translate([0, height, 0]) {
        Pins(width, wood_width, beginnings[1], ends[1]);
        if (corners) {
            translate([width, 0, 0]) 
            cube([wood_width, wood_width, wood_width]);
        }
    }
    translate([0, 0, 0]) {
        rotate([0, 0, 90]) {
            Pins(height, wood_width, beginnings[2], ends[2]);
            if (corners) {
                translate([height, 0, 0]) 
                cube([wood_width, wood_width, wood_width]);
            }
        }
    }
    translate([width + wood_width, 0, 0]) {
        rotate([0, 0, 90]) {
            Pins(height, wood_width, beginnings[3], ends[3]);
            if (corners) {
                translate([-wood_width, 0, 0]) 
                cube([wood_width, wood_width, wood_width]);
            }
        }
    }
}

module Top() {
    difference() {
        Wall(BOX_WIDTH, BOX_DEPTH, WOOD_WIDTH, false, [2, 2, 2, 2], [0, 0, 0, 0]);
        translate([(BOX_WIDTH - PRINTER_WIDTH) / 2, (BOX_DEPTH - PRINTER_DEPTH) / 2 + 1, -.1]) {
            cube([PRINTER_WIDTH, PRINTER_DEPTH, WOOD_WIDTH + 0.2]);
        }
        translate([(BOX_WIDTH) / 2, (BOX_DEPTH) / 2 - 3.5, -.1]) {
            cylinder(d=BUTTON_DIAMETER, $fn=100);
        }
    }
}

module Bottom() {
    difference() {
        Wall(BOX_WIDTH, BOX_DEPTH, WOOD_WIDTH, false, [2, 2, 2, 2], [0, 0, 0, 0]);
        diam = min(BOX_WIDTH, BOX_DEPTH) - 2;
        translate([BOX_WIDTH / 2, BOX_DEPTH / 2, -.1]) {
            cylinder(d=diam, h=WOOD_WIDTH + 1, $fn=100);
        }
    }
}

module Front() {
    Wall(BOX_WIDTH, BOX_HEIGHT, WOOD_WIDTH, false, [0, 0, 2, 2], [0, 0, -2, -2]);
}

module Back() {
    difference() {
        Wall(BOX_WIDTH, BOX_HEIGHT, WOOD_WIDTH, false, [0, 0, 2, 2], [0, 0, -2, -2]);
        translate([2, 2, -.1]) {
            cylinder(d=1, h=WOOD_WIDTH + 1, $fn=100);
        }
    }
}


module Left() {
    Wall(BOX_HEIGHT, BOX_DEPTH, WOOD_WIDTH, true, [0, 0, 0, 0], [0, 0, 0, 0]);
}

module Right() {
    Wall(BOX_HEIGHT, BOX_DEPTH, WOOD_WIDTH, true, [0, 0, 0, 0], [0, 0, 0, 0]);
}

module 3D() {
    translate([0, 0, BOX_HEIGHT + WOOD_WIDTH]) {
        color("blue")
        Top();
    }

    // Bottom with M-Cutout
    translate([0, 0, 0]) {
        color("red")
        Bottom();
    }

    translate([0, 0, WOOD_WIDTH]) {
        rotate([90, 0, 0]) {
            color("yellow")
            Front();
        }
    }

    translate([0, BOX_DEPTH + WOOD_WIDTH, WOOD_WIDTH]) {
        rotate([90, 0, 0]) {
            color("green")
            Back();
        }
    }

    translate([-WOOD_WIDTH, 0, BOX_HEIGHT + WOOD_WIDTH]) {
        rotate([0, 90, 0]) {
            color("orange")
            Left();
        }
    }

    translate([BOX_WIDTH, 0, BOX_HEIGHT + WOOD_WIDTH]) {
        rotate([0, 90, 0]) {
            color("pink")
            Right();
        }
    }
}

module 2D() {
    translate([0, BOX_WIDTH + WOOD_WIDTH * 3, 0]) {
        color("blue")
        Top();
    }

    translate([0, 0, 0]) {
        color("red")
        Bottom();
    }

    translate([BOX_WIDTH + WOOD_WIDTH * 3, 0, 0]) {
        color("yellow")
        Left();
    }

    translate([BOX_WIDTH + WOOD_WIDTH * 3, BOX_WIDTH + WOOD_WIDTH * 3, 0]) {
        color("green")
        Right();
    }

    translate([2 * BOX_HEIGHT + BOX_WIDTH + WOOD_WIDTH * 6, 0, 0]) {
    // translate([0, 0, 0]) {
        color("orange")
        rotate([0, 0, 90])
        Front();
    }

    translate([2 * BOX_HEIGHT + BOX_WIDTH + WOOD_WIDTH * 6, BOX_WIDTH + WOOD_WIDTH * 3, 0]) {
    // translate([0, BOX_WIDTH + WOOD_WIDTH * 3, 0]) {
        color("pink")
        rotate([0, 0, 90])
        Back();
    }
}
