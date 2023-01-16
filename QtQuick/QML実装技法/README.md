
# –Ê”’‚¢ŽÀ‘••û–@


## eval‚Ý‚½‚¢‚ÈŽÀ‘•‚ðQML‚ÅŽg‚¤

```
import QtQuick 2.12
MouseArea {
    id: root
    width: 100
    height: 100

    property string a0: 'value of a0'
    property string a1: 'value of a1'
    property string a2: 'value of a2'

    Column {
        anchors.centerIn: parent
        Repeater {
            model: 3
            Text {
                text: root['a%1'.arg(model.index)]
            }
        }
    }
    onClicked: Qt.quit()
}
```

