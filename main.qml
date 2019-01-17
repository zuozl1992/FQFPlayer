import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.platform 1.1
import QtQuick.Controls 1.4

Window {
    function openMedia(path,type)
    {
        mainPage.isFirstPic = true
        img.source = "image://Color/black"
        img.visible = false
        if(!VideoControl.openMedia(path,type))
        {
            headAni2.running = true
            footAni2.running = true
            headInfo.text = "打开失败"
            pausePlay.imagePath = "qrc:/images/stop.png"
            return;
        }
        hideTimer.running = true
        if(type === 1)
        {
            spectrum.visible = true
            spectrumPoint.visible = true
            lrcItem.visible = true
        }
        else
        {
            spectrum.visible = false
            spectrumPoint.visible = false
            lrcItem.visible = false
        }

        pausePlay.imagePath = "qrc:/images/pause.png"
        video.videoWidth = VideoControl.getVideoWidth();
        video.videoHeight = VideoControl.getVideoHeight();
        var filename = String(path).split("/")[String(path).split("/").length-1]
        var arr = String(filename).split(".")
        var str = ""
        for(var i = 0; i < arr.length-1; i++)
        {
            if(i != 0)
                str += "."
            str += arr[i]
        }

        headInfo.text = str
        headAni1.running = true
        footAni1.running = true
    }

    id: mainPage
    visible: true
    width: 1024
    height: 768
    minimumWidth: 600
    minimumHeight: 600
    property bool isChangeSlider: true
    property bool specLeftEnable: true
    property bool specRightEnable: true
    property bool paintEnable: true
    property bool isFirstPic: true
    title: qsTr("FQFVidePlayer")

    onXChanged: {
        mainPage.paintEnable = false
    }
    onYChanged: {
        mainPage.paintEnable = false
    }

    Timer {
        running: true
        interval: 100
        repeat: true
        onTriggered: {
            specLeftEnable = true
            specRightEnable = true
            paintEnable = true
        }
    }

    Connections {
        target: VideoControl
        onCallQmlRefeshImg: {
            img.visible = true
            img.source = "image://CodeImg/"+ Math.random()
            if(mainPage.isFirstPic)
            {
                smallImg.source = "image://CodeImg/"+ Math.random()
                mainPage.isFirstPic = false
            }
        }
        onNewColor: {
            for(var i = 0; i < 256; i++)
            {
                spectRep.itemAt(i).color = arr[i]
            }
        }

        onNewLeftSpectrum: {
            if(!mainPage.paintEnable)
                return
            if(!mainPage.specLeftEnable)
                return;
            var i;
            for(i = 128; i < 256; i++)
                spectRep.itemAt(i).height =
                        (arr[i-127] * 18 > mainPage.height / 3) ? mainPage.height / 3 : arr[i-127] * 18
            for(i = 0; i < 128; i++)
                spectRep.itemAt(i).height =
                        (arr[128-i] * 18 > mainPage.height / 3) ? mainPage.height / 3 : arr[128-i] * 18
            mainPage.specLeftEnable = false
        }
        onNewRightSpectrum: {
            if(!mainPage.paintEnable)
                return
            if(!mainPage.specRightEnable)
                return;
            var i;
            for(i = 128; i < 256; i++)
            {
                spectRepPoint.itemAt(i).anchors.bottomMargin =
                        (arr[i-127] * 18 > mainPage.height / 3) ? mainPage.height / 3 : arr[i-127] * 18 + 1
            }
            for(i = 0; i < 128; i++)
                spectRepPoint.itemAt(i).anchors.bottomMargin =
                        (arr[128-i] * 18 > mainPage.height / 3) ? mainPage.height / 3 : arr[128-i] * 18 + 1
            mainPage.specRightEnable = false
        }
    }

    Timer {
        id: hideTimer
        interval: 2000; running: false; repeat: true
        onTriggered:
        {
            headAni1.running = true
            footAni1.running = true
        }
    }

    Timer {
        id: updateTimer
        interval: 100; running: true; repeat: true
        onTriggered: {
            if(!isChangeSlider)
                return
            var pos = VideoControl.getMediaPos()
            videoPosPro.value = pos
        }
    }

    Row {
        id: spectrumPoint
        width: parent.width - 2 * spacing
        height: parent.height / 3 + 20
        anchors.left: parent.left
        anchors.leftMargin: spacing
        anchors.bottom: foot.top
        spacing: ((parent.width - 257 * 2) / 256) > 3 ? (parent.width - 3 * 256) / 257 : 2
        visible: false
        z: 5


        Repeater {
            id: spectRepPoint
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            width: spectrumPoint.width
            height: spectrumPoint.height
            model: 256
            Rectangle {
                width: ((spectrumPoint.parent.width - 257 * 2) / 256) > 3 ? 3 : (spectrumPoint.parent.width - 257 * 2) / 256
                height: width
                anchors.bottom: spectRepPoint.bottom
                anchors.bottomMargin: 20
                color: "purple"
                radius: width/2
            }
        }
    }

    Row {
        id: spectrum
        width: parent.width
        height: parent.height / 3
        anchors.left: parent.left
        anchors.bottom: foot.top
        spacing: 1
        visible: false
        z: 4
        Repeater {
            id: spectRep
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            width: spectrum.width
            model: 256
            Rectangle {
                width: (spectrum.width - 257) / 256
                height: 0
                anchors.bottom: spectRep.bottom
                color: "green"
                radius: width/2
            }
        }
    }

    Rectangle {
        id: head
        width: parent.width
        height: 30
        x: 0
        y: 0
        z: 3
        color: "#3f3f3f"
        PropertyAnimation {
            id: headAni1
            target: head
            properties: "y"
            to: -30
            duration: 500
        }
        PropertyAnimation {
            id: headAni2
            target: head
            properties: "y"
            to: 0
            duration: 500
            onStarted: {
                hideTimer.stop()
            }
            onFinished: {
                hideTimer.restart()
            }
        }
        Text {
            id: headInfo
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.top: parent.top
            height: parent.height
            width: parent.width / 2 - 24
            font.pointSize: 12
            text: qsTr("")
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: "white"
            z: 2
        }
        Text {
            id: openFileBtn
            anchors.left: headInfo.right
            anchors.leftMargin: 8
            anchors.top: parent.top
            height: parent.height
            width: (parent.width / 2 - 24) / 2 - 8
            font.pointSize: 12
            text: qsTr("打开文件")
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: "white"
            z: 3
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fileDialog.open()
                }
            }
        }
        Text {
            id: openStreamBtn
            anchors.left: openFileBtn.right
            anchors.leftMargin: 8
            anchors.top: parent.top
            height: parent.height
            width: (parent.width / 2 - 24) / 2 - 8
            font.pointSize: 12
            text: qsTr("打开串流")
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: "white"
            z: 3
            MouseArea {
                anchors.fill: parent
                onClicked: {
                }
            }
        }
    }

    Item {
        id: foot
        width: parent.width
        height: 62
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        z: 3
        PropertyAnimation {
            id: footAni1
            target: foot
            properties: "anchors.bottomMargin"
            to: -62
            duration: 500
        }
        PropertyAnimation {
            id: footAni2
            target: foot
            properties: "anchors.bottomMargin"
            to: 0
            duration: 500
            onStarted: {
                hideTimer.stop()
            }
            onFinished: {
                hideTimer.restart()
            }
        }

        Slider {
            id: videoPosPro
            width: parent.width
            height: 22
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            maximumValue: 1
            minimumValue: 0
            value: 0
            onPressedChanged: {
                if(pressed)
                {
                    isChangeSlider = false
                }
                else
                {
                    headAni2.running = false
                    footAni2.running = false
                    VideoControl.seek(value)
                    headAni2.running = true
                    footAni2.running = true
                    isChangeSlider = true
                }
            }
        }
        BtnImg {
            id: pausePlay
            anchors.left: parent.left
            anchors.leftMargin: (parent.width - width)/2
            anchors.bottom: videoPosPro.top
            imagePath: "qrc:/images/stop.png"
            property bool isPause: false
            width: 40
            height: 40
            onClicked: {
                VideoControl.setPause(!isPause)
                isPause = !isPause
                imagePath = !isPause ? "qrc:/images/pause.png" : "qrc:/images/play.png"
            }
        }
    }

    Rectangle {
        id: video
        color: "black"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        z: 2
        property double videoWidth: 0
        property double videoHeight: 0
        //0--无  1--窄视频  2--宽视频
        property double videoPropStatus: (videoHeight <= 0) ?
                                             0 :
                                             (((width / height) > (videoWidth / videoHeight)) ? 1 : 2)
        Image {
            id: img
            width: parent.videoPropStatus == 0 ? 0 : parent.videoPropStatus == 1 ?
                           (parent.height / parent.videoHeight * parent.videoWidth) : parent.width
            height: parent.videoPropStatus == 0 ? 0 : parent.videoPropStatus == 1 ?
                        parent.height : (parent.width / parent.videoWidth * parent.videoHeight)
            anchors.left: parent.left
            anchors.leftMargin: (parent.width - width) / 2
            anchors.top: parent.top
            anchors.topMargin: (parent.height - height) / 2
            z: 2
            visible: false
            Item {
                id: lrcItem
                visible: false
                anchors.fill: parent
                Rectangle {
                    anchors.fill: parent
                    color: "black"
                    opacity: 0.8
                    z: 2
                }
                Image {
                    id: smallImg
                    width: 120
                    height: 120
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 50
                    anchors.topMargin: 50
                    z: 3
                }
            }
        }
        Image {
            id: logo
            width: 300
            height: 300
            anchors.centerIn: parent
            source: "qrc:/images/logo_player2.png"
            visible: !img.visible
            z: 3
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fileDialog.open()
                }
            }
        }


        MouseArea {
            z: 2
            anchors.fill: parent
            anchors.margins: 60
            onClicked: {
                if(head.y == -30 || foot.y == foot.parent.height)
                {
                    headAni2.running = true
                    footAni2.running = true
                }
                else
                {
                    headAni1.running = true
                    footAni1.running = true
                }
            }
        }
    }

    FileDialog {
        id: fileDialog;
        title: qsTr("Please choose an image file");
        selectedNameFilter.index: 0
        nameFilters: [
            "Video Files (*.mp4 *.mov)",
            "Audio Files (*.mp3 *.flac)"
        ];
        onAccepted: {
            var path = file.toString()
            var cs;
            if(Qt.platform.os == "windows"){
                cs = path.slice(8);
            }else{
                cs = path.slice(7);
            }
            openMedia(cs,selectedNameFilter.index)
        }

    }
}
