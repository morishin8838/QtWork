/*
* 機能: V4L2 を使用して UVC カメラ データを収集します。
* 使用法: Ubuntu の下の Qt プロジェクトに V4L2.h および V4L2.cpp ファイルを追加し、V4L2 オブジェクトを定義します。
* bool V4L2::V4l_Init(char* camera_path, unsigned int frame) 関数を呼び出して、カメラを初期化します
* QImage V4L2::Get_image() 関数を呼び出して、カメラから画像を取得します
* bool Close_Camera(void) 関数を呼び出してカメラを閉じます
※注意：V4L2.cppの54行目あたりでカメラ画像の出力形式をMJPEG出力に変更するなどの変更が必要です
※V4L2.hのマクロ定義で、出力画像のピクセルを変更可能
*/

#ifndef V4L2_H
#define V4L2_H

#include <fcntl.h>                                  /** カメラのオープン関数が配置されているヘッダー ファイルを開きます */
#include <sys/mman.h>                               /** 適用されたカーネル キャッシュをユーザー空間の mmap関数のヘッダー ファイルにマップします */
#include <linux/videodev2.h>                        /** V4L2 関連の構造体が配置されているヘッダー ファイル */
#include <unistd.h>                                 /** カメラのクローズ関数が配置されているヘッダー ファイルを閉じます */
#include <sys/ioctl.h>
#include <QDebug>
#include <QImage>

#define Video_count    3                            /** バッファ フレーム数 */
#define Image_Width    1280                         /** 出力画像のピクセル数 */
#define Image_High     720

#define USE_DEVIDE     "/dev/video0"                /** v4l2-ctl --list-devices         利用できるWebカメラの一覧を取得する **/
                                                    /** v4l2-ctl -d /dev/video0 --info  Webカメラの情報やフォーマットを調べる **/
#define V4L2_FPS       30

class V4L2
{
public:
    V4L2();
    ~V4L2();
    bool V4l_Init(char *camera_path, unsigned int frame);/** カメラのマウント パスと出力フレーム レートを渡す必要があるカメラ初期化関数で、初期化が成功した場合は true を返します */
    QImage Get_image(void);                             /** 画像を取得、戻り値は QImage 型の変数 */
    bool Close_Camera(void);                            /** カメラを閉じます。成功した場合は true を返します */
    int n;                                              /** Get_image 関数で、画像を取得するときにどのキャッシュを制御するかを制御するために使用されます */

private:
    int                           i;
    int                           fd;                   /** カメラ ハンドル */
    int                           length[Video_count];  /** アプリケーションの保存に使用されるキャッシュのサイズ */
    QImage                        image;
    unsigned char *               start[Video_count];/** 画像データの保存に使用 */

    struct v4l2_buffer            buffer[Video_count];/** この構造体はカーネルからキャッシュを申請する際に使用され、各 struct v4l2_buffer はカーネル カメラ ドライバ内のキャッシュに対応します */
    struct v4l2_format            fmt;               /** ピクセル形式の画像出力形式の画像サイズのスキャン方法を設定するために使用されます */
    struct v4l2_fmtdesc           fmtdesc;           /** この構造体は、カメラ出力画像でサポートされている形式を取得するために必要です */
    struct v4l2_capability        cap;               /** カメラ情報を確認する際に必要な構造体 */
    struct v4l2_streamparm        setfps;            /** この構造体は、フレーム レートを設定するときに使用されます */
    struct v4l2_requestbuffers    req;               /** この構造体は、カーネルからキャッシュを申請するときに使用されます */
    struct v4l2_buffer            v4lbuf;            /** キャッシュのデキュー この構造体は、キューに入るときに使用されます */
    enum   v4l2_buf_type          type;              /** この構造体は、I/O ストリームを開くときに使用されます */
};

#endif // V4L2_H
