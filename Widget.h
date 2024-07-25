#pragma once

#include <thread>
#include <QTextCodec>
#include <qapplication.h>
#include <QWidget>
#include <qmovie.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <vector>
#include <qtimer.h>
#include <random>
#include <qgraphicseffect.h>
#include <qmouseeventtransition.h>
#include <QMouseEvent>  // 鼠标事件
#include <QWheelEvent>  // 滚轮事件
#include <qpushbutton.h>
#include <memory>
#include <qpainter.h>
#include <qvector.h>
#include <stdlib.h>
#include <qthread.h>
#include <qdebug.h>
#include <dirent.h>  // 文件操作
#include <qmediaplayer.h>  // 播放音频
#include <chrono>
#include <mutex>
#include <fstream>
#include <string>
#include <QLineEdit>
#include <qtextedit.h>
#include <QVBoxLayout>
#include <ratio>
#include <qslider.h>
#include <qdatetime.h>
#include <math.h>

#include "MY_DEFINE.h"
#include "sWidget.h"
#include "func_tools.h"

class Widget : public QWidget
{
	Q_OBJECT  // 声明宏

public:
	Widget(QWidget* parent = nullptr);
	~Widget();

	void showGif(qint8 path_id = path_id);  // 展示动画

	void randOpenVoice();  // 随机打开语音

	bool eventFilter(QObject* watched, QEvent* ev) override;  // 鼠标事件
	void wheelEvent(QWheelEvent* event) override;  // 重写滚轮事件
	void paintEvent(QPaintEvent* event) override;  // 重写绘制事件
	void scaleWidgets(unsigned char scale_history);  // 所有窗口调整大小

	void hideAllModuel();  // 隐藏组件
	void showAllModuel();  // 显示组件

	static qint16 getRandValue(qint16 range);  // 获取随机值

	void testNicknameLineChanged();

	void volumeControlFadeIn(QMediaPlayer*& device);  // 淡入

	void volumeControlFade(QMediaPlayer*& device);  // 淡出


signals:
	void dialog_text_changed();  // 文本框改变信号
signals:
	void nickname_line_changed(QString nickname_temp);  // 名字改变信号

private:
	QLabel* show_gif_label;// 贴图框架
	std::vector<QMovie*> show_gif_anime;  // gif贴图对象(状态图)
	//QGraphicsDropShadowEffect* widget_shadow_effect;  // 阴影效果对象

	QPushButton* paint_button;  // 触摸轮廓可视性按钮
	QPushButton* close_pet_button;
	static bool close_flag;
	QPixmap* close_pixmap;  // 按钮图片
	std::vector<QPainter*> draw_sensitive;  // 显示感兴趣区域

	QMediaPlayer* rand_voice_player;  // 随机语音播放
	QMediaPlayer* touch_audio_player;  // 触碰语音播放(优先级高于随机语音播放)
	bool acc_interrupt;  // 意外中断

	QPushButton* rand_voice_button;
	QSlider* rand_voice_slider;

	QPushButton* rand_voice_next;

	QPushButton* rand_voice_pause;
	bool rvoice_pause_flag;  // 声音暂停判断
	qint8 volume_value;  // 音量响度

	QPushButton* rand_voice_prev;
	qint16 his_rand;  // 历史音频播放序号

	std::mutex voice_mtx;  // 音频锁
	qint64 dx_rand_voice;  // 微分值
	qint64 dx_pause_value;  // 暂停的微分值

	QPushButton* nickname_button;  // 绰号签名
	static bool nickname_flag;
	QLineEdit* nickname_content;

	QTextEdit* dialog_frame;  // 对话框
	std::string rand_voice_text;  // 随机语音文字

	qint8 judgeDirection(QPoint& p1, QPoint& p2);  // 判断方向

public:
	static bool right_mouse;  // 右键是否按下判断
	static bool drap_show_flag;  // 拖动显示
	static qint8 history_catch_value;
	static std::vector<QString> path_name;// gif路径字符串
	static const std::vector<qint8> judge_gif_loop;  // loop动画和limit动画
	static std::vector<bool> gif_over;
	static qint8 judge_loop_frequency;

	static qint8 gif_state_num;  // gif状态序号
	static qint8 path_id;  // 路径显示序号

	static std::vector<QRect> sensitive_area;  // 三月的敏感区域
	static std::vector<double> scale_info;  // 缩放表
	static qint8 scale_variable;  // 缩放变量

	static bool rand_voice_flag;
	static qint16 voice_dir_num;  // 声音文件的数量

	// 晃晕状态标记
	qint64 move_stun_timer;  // 晃晕时间计数器
	qint8 jugde_direction;  // 判断方向
	qint8 direction_change_counter;  // 方向变化计数器
	QPoint history_position;  // 历史点位
};

qint16 count_files_in_directory(const std::string& path);

extern QString user_name;



