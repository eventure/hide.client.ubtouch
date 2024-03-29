# hide.client.ubtouch
Hide.me Ubuntu Touch Client

# build instucions
## Clickable packages
### install clickable
Add the PPA to your system: 
```sudo add-apt-repository ppa:bhdouglass/clickable```
Update your package list: 
```sudo apt-get update```
Install clickable: 
```sudo apt-get install clickable```
### build
For building for device run:
``` clickable --build --arch arm64 ```

## For test on Desktop
### install lomiri-ui-components
         qml-module-lomiri-content
         qml-module-qtquick2
         qml-module-lomiri-components
         qml-module-lomiri-layouts
         qml-module-qtquick-window2
#build
```cd $SOUECE DIR```
```mkdir build```
```cd build```
```cmake ../ ```
``` make ```
