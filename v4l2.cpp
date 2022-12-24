/*
* 機能: V4L2 を使用して UVC カメラ データを収集します。
* 使用法: Ubuntu の下の Qt プロジェクトに V4L2.h および V4L2.cpp ファイルを追加し、V4L2 オブジェクトを定義します。
* bool V4L2::V4l_Init(char* camera_path, unsigned int frame) 関数を呼び出して、カメラを初期化します
* QImage V4L2::Get_image() 関数を呼び出して、カメラから画像を取得します
* bool Close_Camera(void) 関数を呼び出してカメラを閉じます
※注意：V4L2.cppの54行目あたりでカメラ画像の出力形式をMJPEG出力に変更するなどの変更が必要です
※V4L2.hのマクロ定義で、出力画像のピクセルを変更可能
*/

#include "v4l2.h"

V4L2::V4L2()
{
    n = 0;
}

V4L2::~V4L2()
{

}

/** カメラを初期化するには、カメラのマウント パスと出力フレーム レートを渡し、初期化が成功した場合は true を返す必要があります */
bool V4L2::V4l_Init(char *camera_path, unsigned int frame)
{
    if((fd=open(camera_path, O_RDWR)) == -1){          /** カメラを開くための読み取りおよび書き込みモード */
        qDebug()<<"Error opening V4L interface";
        return false;
    }
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1){       /** カメラ情報を確認する */
        qDebug()<<"Error opening device "<<camera_path<<": unable to query device.";
        return false;
    }else{                                             /** 打印摄像头的信息 */
        qDebug()<<"driver:\t\t"<<QString::fromLatin1((char *)cap.driver);     /** ドライバー名 */
        qDebug()<<"card:\t\t"<<QString::fromLatin1((char *)cap.card);         /** 装置名 */
        qDebug()<<"bus_info:\t\t"<<QString::fromLatin1((char *)cap.bus_info);
        qDebug()<<"version:\t\t"<<cap.version;
        qDebug()<<"capabilities:\t"<<cap.capabilities;                        /** V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING */
    }

    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;          /** struct v4l2_format の型と一致 **/
    qDebug()<<"Support format:";
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1){     /** カメラ出力画像がサポートする形式を取得する */
        qDebug()<<"\t\t"<<fmtdesc.index+1<<QString::fromLatin1((char *)fmtdesc.description);
        fmtdesc.index++;
    }

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;            /** ピクセル形式  */
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;      /** JPEG形式出力 */
    fmt.fmt.pix.height = Image_High;                   /** 画像サイズ、出力したい画像の幅と高さをここで設定 */
    fmt.fmt.pix.width = Image_Width;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;         /** ビデオフレーム伝送方式、インターレース */
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1){           /** カメラのキャプチャ形式を構成する */
        qDebug()<<"Unable to set format";
        return false;
    }
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1){           /** 構造を読み直し、設定が完了していることを確認する */
        qDebug()<<"Unable to get format";
        return false;
    }else{
        qDebug()<<"fmt.type:\t\t"<<fmt.type;            /** 出力ピクセル形式 */
        qDebug()<<"pix.height:\t"<<fmt.fmt.pix.height;  /** 出力画像サイズ */
        qDebug()<<"pix.width:\t\t"<<fmt.fmt.pix.width;
        qDebug()<<"pix.field:\t\t"<<fmt.fmt.pix.field;  /** 映像フレーム伝送方式 */
    }

    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps.parm.capture.timeperframe.denominator = frame;   /** 期待されるフレームレート */
    setfps.parm.capture.timeperframe.numerator = 1;         /** fps=frame/1 */
    if(ioctl(fd, VIDIOC_S_PARM, &setfps)==-1){              /** フレーム数を設定する */
        qDebug()<<"Unable to set fps";
        return false;
    }
    if(ioctl(fd, VIDIOC_G_PARM, &setfps)==-1){              /** 構造を読み直し、設定が完了していることを確認する */
        qDebug()<<"Unable to get fps";
        return false;
    }else{
        qDebug()<<"fps:\t\t"<<setfps.parm.capture.timeperframe.denominator/setfps.parm.capture.timeperframe.numerator;/** 输出帧率 */
    }

    req.count=Video_count;                              /** 3つのキャッシュ */
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;               /** struct v4l2_format の型と一致 */
    req.memory=V4L2_MEMORY_MMAP;                        /** メモリ マッピング モード カウント フィールドは、V4L2_MEMORY_MMAP に設定されている場合にのみ有効です */
    if(ioctl(fd,VIDIOC_REQBUFS,&req)==-1){              /** ビデオキャッシュをカーネルに申請 */
        qDebug()<<"request for buffers error";
        return false;
    }
    for (i=0; i<Video_count; i++){                      /** mmap 4 バッファー */
        bzero(&buffer[i], sizeof(buffer[i]));
        buffer[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer[i].memory = V4L2_MEMORY_MMAP;
        buffer[i].index = i;
        if (ioctl (fd, VIDIOC_QUERYBUF, &buffer[i]) == -1){ /** バッファのパラメータを取得します。これらのパラメータは mmap に必要です */
            qDebug()<<"query buffer error";
            return false;
        }
        length[i] = buffer[i].length;                   /** キャッシュサイズを保存 */
        start[i] = (unsigned char *)mmap(NULL,buffer[i].length,PROT_READ |PROT_WRITE, MAP_SHARED, fd, buffer[i].m.offset);
                                                        /** 要求されたカーネル キャッシュをユーザー空間にマップする */
    }
    for (i=0; i<Video_count; i++){
        buffer[i].index = i;
        ioctl(fd, VIDIOC_QBUF, &buffer[i]);             /** キャッシュをキューに入れる */
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl (fd, VIDIOC_STREAMON, &type);                 /** I/O ストリームを開く */

    bzero(&v4lbuf, sizeof(v4lbuf));
    v4lbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4lbuf.memory = V4L2_MEMORY_MMAP;
    qDebug()<<"init ok";

    return true;
}

QImage V4L2::Get_image()                               /** カメラでキャプチャされた画像を取得し、戻り値は QImage 型の変数です */
{
    v4lbuf.index = n%Video_count;
    ioctl(fd, VIDIOC_DQBUF, &v4lbuf);                  /** 画像データを取得するバッファをデキューします */

    QByteArray temp;
    temp.append((const char *)start[n%Video_count],length[n%Video_count]);
    image.loadFromData(temp);                          /** 画像データを画像変数に入れる */

    v4lbuf.index = n%Video_count;
    ioctl(fd, VIDIOC_QBUF, &v4lbuf);                   /** すでに読み取られたキャッシュ ブロックを再度キューに入れる */
    n++;
    if(n == 3){                                        /** n累積オーバーフローを防ぐ */
        n = 0;
    }
    return image;                                      /** 画像データを返す */
}

bool V4L2::Close_Camera()                              /** カメラを閉じます。成功した場合は true を返します */
{
    if(fd != -1){
        ioctl(fd, VIDIOC_STREAMOFF, &type);            /** 終了画像表示 */
        int n = close(fd);                             /** ビデオ機器をオフにする */
        if(n == -1){
            qDebug()<<"close camera failed";
            return false;
        }
    }
    for(i=0; i<Video_count; i++){
        if(start[i] != NULL){                          /** 要求されたメモリを解放する */
            start[i] = NULL;
        }
    }
    return true;
}
