// Image Wall example
//
// Example usage of Dali API
//
//
//
// get the dali-demo image directory path
// hard code for the device to /usr/apps/com.samsung.dali-demo/images/
var imageDir = dali.DALI_DATA_DIRECTORY;

if (imageDir != "/usr/share/dali//") {
    imageDir = imageDir.substring(0, imageDir.lastIndexOf("dali/"));
    imageDir += "com.samsung.dali-demo/images/";
} else // on device
{
    imageDir = "/usr/apps/com.samsung.dali-demo/images/";
}


var NUMBER_OF_IMAGES = 40; // for now use 16 ( demo files go up to 30)
var DEMO_IMAGES = []; // array to store Dali Images
var VIDEO_WALL_ACTORS = []; // array to store Image actors
var VIDEO_WALL_ROWS = 7; // use 3 rows for the video wall
var VIDEO_WALL_COLUMNS = 12; // use 12 columns for the video wall
var VIDEO_WALL_TOTAL_ITEMS = VIDEO_WALL_COLUMNS * VIDEO_WALL_ROWS; // total items
var VIDEO_WALL_ITEM_SIZE = 128; // width / height of a item in the video wall
var BORDER_SIZE = 5;
var VIDEO_WALL_ITEM_SIZE_NO_BORDER = VIDEO_WALL_ITEM_SIZE - BORDER_SIZE;
var VIDEO_WALL_WIDTH = VIDEO_WALL_COLUMNS * VIDEO_WALL_ITEM_SIZE;
var VIDEO_WALL_HEIGHT = VIDEO_WALL_ROWS * VIDEO_WALL_ITEM_SIZE;

var daliApp = {};

var wallRootActor; // the root actor of the video wall

// we want demo images of format gallery-small-1.jpg
daliApp.getFileName = function(index) {
    fileName = "gallery-small-" + (index+1) + ".jpg";
    return fileName;
}

// load the images
daliApp.loadImages = function() {
    for (index = 0; index < NUMBER_OF_IMAGES; ++index) {
        fileName = imageDir + daliApp.getFileName(index);
        DEMO_IMAGES[index] = new dali.ResourceImage( { url:fileName } );
    }
}

daliApp.createRootActor = function() {
    wallRootActor = new dali.Actor();
    wallRootActor.parentOrigin = dali.CENTER;
    wallRootActor.anchorPoint = dali.CENTER;
    dali.stage.add(wallRootActor);
}



daliApp.getWallActorIndex = function(x, y) {
    return x + y * VIDEO_WALL_COLUMNS;
}

daliApp.createActors = function() {
    daliApp.createRootActor();

    for (y = 0; y < VIDEO_WALL_ROWS; ++y) {
        for (x = 0; x < VIDEO_WALL_COLUMNS; ++x) {

            var actorIndex = daliApp.getWallActorIndex(x, y);
            var imageActor = new dali.ImageActor();

            // wrap image index between 0 and NUMBER_OF_IMAGES
            var imageIndex = actorIndex % NUMBER_OF_IMAGES;

            imageActor.setImage(DEMO_IMAGES[imageIndex]);

            imageActor.parentOrigin = dali.CENTER;
            imageActor.anchorPoint = dali.CENTER;
            imageActor.size = [VIDEO_WALL_ITEM_SIZE_NO_BORDER, VIDEO_WALL_ITEM_SIZE_NO_BORDER, 1.0]; // start with zero size so it zooms up

            var xPosition = x * VIDEO_WALL_ITEM_SIZE;
            //  as the middle the wall is at zero (relative to wallRootActor), we need to subtract half the wall width.
            // + add half item size because the item anchor point is the center of the wallRootActor.
            xPosition = xPosition - (VIDEO_WALL_WIDTH / 2) + (VIDEO_WALL_ITEM_SIZE / 2);

            var yPosition = y * VIDEO_WALL_ITEM_SIZE;
            yPosition = yPosition - (VIDEO_WALL_HEIGHT / 2) + (VIDEO_WALL_ITEM_SIZE / 2);

            imageActor.position = [xPosition, yPosition, 0.0];
            // store the actor
            VIDEO_WALL_ACTORS[actorIndex] = imageActor;

            // Add to the video wall root actor.
            wallRootActor.add(imageActor);
        }
    }
}

function Initialise() {

    daliApp.loadImages();

    daliApp.createActors();


}

Initialise();
