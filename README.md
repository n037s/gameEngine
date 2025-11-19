Project on Visual Studio 2022.

For now we can display a window, and manage to add some objects.
Also the camera is managed and can be controlled by keyboard inputs.



Starting code is in Viewer/Viewer/main.cpp



WHAT's new : 

World serialization and deserialization. 



TO BE DONE :

* The first new executable project will be the world editor : displaying world, can add items from assets.
* It is needed to be able to access some items from the world by an identifiant ? Some object could be registered as specifics items. 
* Z management is not the best. Could be great to only sort items by height that are to be renderer. All items in rendering rect are extracted from all list and they are then sorted before being rendered ?
* Add an item drop down button. It will appear with a right click on the window, it will be able to create any item in the asset manager at a position.
* Add bounding rect Object that can be linked to an object and modify its position and size. 
* Left click will make appear a bounding rect linked to the item clicked on.
* Far later : should be able to modify object properties ( animated frames, button callback, text label)
