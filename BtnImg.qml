import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Button {
    id: trafficBtn
    property string imagePath
    style: ButtonStyle
    {
        background: Image {
            anchors.fill: parent
            anchors.margins: control.hovered ? (control.pressed ? 2 : 0) :  3
            source: imagePath
        }
    }
}
