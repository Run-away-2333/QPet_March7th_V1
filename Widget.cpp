#include "Widget.h"

QString user_name = "";  // 初始化用户id

// 编译宏
constexpr int DX = DX_RAND_VOICE;  // 微分量
constexpr qint64 RAND_VOICE_TIME_SLEEP = RAND_VOICE_SLEEP;
constexpr int _DIALOG_FONT_SIZE = DIALOG_FONT_SIZE;
constexpr int _LINE_FONT_SIZE = LINE_FONT_SIZE;
constexpr int _TOUCH_AUDIO_NUMS = TOUCH_AUDIO_NUMS;
constexpr int _GIF_WIDTH = GIF_WIDTH;
constexpr int _GIF_HEIGHT = GIF_HEIGHT;

bool Widget::right_mouse = false;
bool Widget::rand_voice_flag = false;

bool Widget::drap_show_flag = INIT_DRAP_TOOL;  // 拖动显示判断
qint8 Widget::history_catch_value = 0;

bool Widget::nickname_flag = false;

bool Widget::close_flag = false;

qint16 Widget::voice_dir_num = count_files_in_directory("./voice/speak");

qint8 Widget::gif_state_num = INIT_GIF_STATE_ID;  // 初始化gif状态序号
qint8 Widget::path_id = INIT_GLOBAL_PATH_ID;

std::vector<QString> Widget::path_name{ "./1/catch_lovely.gif", "./1/catch_doubt.gif", "./1/lovely_stun.gif",
	"./1/doubt_stun.gif", "./1/common_sit1.gif", "./1/sumg_sit.gif",
	"./1/compolish_sit.gif","./1/embrass_smile_sit.gif", "./1/failed_sit.gif",
	"./1/happy_sit.gif", "./1/hold_jaw_sit.gif", "./1/win_sit.gif",
	"./1/slack_sit.gif", "./1/akimbo_sit.gif", "./1/left_eight_sit.gif",
	"./1/common_stand.gif" };  // gif路径字符串

const std::vector<qint8> Widget::judge_gif_loop{ 0, 0, 1, 1, 0, 10, 1, 1, 1, 0,
1, 1, 0, 1, 1, 0 };  // 写入是否为loop动画(0死循环，1以上为播放次数)

std::vector<bool> Widget::gif_over{ true, true, true, true, true, true, true, true,
true, true, true, true, true, true, true, true };  // gif播放完信号

qint8 Widget::judge_loop_frequency = 0;

std::vector<double> Widget::scale_info{ 0.5, 0.6, 0.7, 0.8, 0.87, 1 };  // 缩放哈希
qint8 Widget::scale_variable = 2;  // 缩放变量

std::vector<QRect> Widget::sensitive_area{ QRect(86, 51, 272, 70), QRect(39, 121, 378, 104),
QRect(17, 224, 128, 167), QRect(145, 224, 273, 96), QRect(145, 320, 260, 35),
QRect(145, 355, 46, 113), QRect(105, 468, 85, 75), QRect(191, 355, 98, 77),
QRect(191, 431, 98, 59), QRect(289, 355, 58, 135), QRect(191, 489, 173, 135) };	 //  初始化敏感区域 0 - 10

Widget::Widget(QWidget* parent)
	: QWidget(parent), show_gif_label(new QLabel(this)), close_pet_button(new QPushButton(this)),
	rand_voice_button(new QPushButton(this)), nickname_button(new QPushButton(this)), dialog_frame(new QTextEdit(this)),
	nickname_content(new QLineEdit(this)), rand_voice_slider(new QSlider(this)), volume_value{ 73 },
	rvoice_pause_flag(false), dx_pause_value(0)
{
	// 去掉窗口边框  
	setWindowFlag(Qt::WindowType::FramelessWindowHint);
	// 透明背景
	setAttribute(Qt::WA_TranslucentBackground);
	// 显示在最上层
	setWindowFlag(Qt::WindowStaysOnTopHint);

	this->installEventFilter(this);  // 注册鼠标点击事件

	acc_interrupt = false;  // 初始化意外中断

	rand_voice_player = new QMediaPlayer(this);
	touch_audio_player = new QMediaPlayer(this);

	// 初始化晃晕器
	move_stun_timer = 0;  // 晃晕时间计数器
	direction_change_counter = 0;  // 方向变化计数器
	history_position = QPoint(0, 0);  // 历史点位
	jugde_direction = 0;

	// 初始化对话文本框

	// 禁用垂直滚动条，确保根据内容自适应大小
	dialog_frame->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	// 设置文本框自动调整大小策略，根据内容调整
	dialog_frame->move(40, 460);
	dialog_frame->resize(249.9, 30.1);  // 30一行
	dialog_frame->setReadOnly(true);  // 只读
	QString qss_text_content("QTextEdit { font-size: 21px; font-family: '大宝桃桃体'; color: rgb(255, 255, 255);"
		"    background: transparent;"
		"    border: 2px solid #c4dff6;"
		"    border-radius: 6px;"
		"    background-image: url(./1/background/dialog_frame.png);"
		"    background-repeat: no-repeat;"
		"    background-position: center;"
		"}");
	dialog_frame->setStyleSheet(qss_text_content);
	dialog_frame->setPlainText("当前无内容呦~");
	connect(this, &Widget::dialog_text_changed, this, [this]()->void {
		QString convert_dialog_text = QString::fromStdString(rand_voice_text).replace("{NICKNAME}", user_name + "♥");
		qint8 dialog_tensor = (convert_dialog_text.toStdString().length() - 3 >= 0 ?
			convert_dialog_text.toStdString().length() - 3 : 0) / 33 + 1;
		dialog_frame->resize(249.9 / 0.7 * scale_info[scale_variable]
			, 30.1 * dialog_tensor / 0.7 * scale_info[scale_variable]);  // 30一行, 一行11个字(33字节)
		dialog_frame->setPlainText(convert_dialog_text);
		});

	// 初始化按钮
	// 更改绰号
	nickname_button->resize(39.9, 39.9);
	nickname_button->move(41, 421);
	nickname_button->setStyleSheet("QPushButton{"
		"border-image: url(./1/button/pen_common.png);}"
		"QPushButton:hover{"
		"border-image: url(./1/button/pen_press_float.png);}");
	nickname_button->setToolTip("点击修改昵称");

	//nickname_content->resize(39.9, 39.9); // 207.9
	// 读文件获得user_name
	std::ifstream file_user_name("./user/user_name.txt");
	if (!file_user_name.is_open()) {
		std::ofstream get_file_user_name("./user/user_name.txt");  // 创建
		get_file_user_name << "老公";  // 默认姓名
		get_file_user_name.close();
		user_name = "老公";
	}
	std::string get_name{ "" };
	std::getline(file_user_name, get_name);
	user_name = QString::fromStdString(get_name);
	file_user_name.close();

	nickname_content->move(81, 421);
	nickname_content->setStyleSheet("QLineEdit { font-size: 28px; font-family: '大宝桃桃体'; "
		"    color: #456e7d; "
		"    background-color: #8db9ca;"
		"    border: 2px solid #c4dff6;"
		"    border-radius: 6px;"
		"}");
	if (!user_name.isEmpty())
		nickname_content->setText(user_name);
	else nickname_content->setText("老公");
	nickname_content->resize(scale_info[scale_variable] * (57 + 20 * (qint8(nickname_content->text().toStdString().length() - 3) <= 0 ?
		1 : nickname_content->text().toStdString().length() - 3)), 57 * scale_info[scale_variable]);
	nickname_content->setMaxLength(8);
	nickname_content->hide();
	// 名字框随输入缩放
	connect(this, &Widget::nickname_line_changed, this, [&](const QString nickname_line_temp) -> void {
		double scale_value = scale_info[scale_variable];
		if (nickname_content->text().toStdString().length() <= 3 ||
			nickname_content->width() / scale_value < 75) {  // min
			nickname_content->resize(78 * scale_value, 57 * scale_value);
		}
		else if (nickname_content->text().toStdString().length() >= 16 ||
			nickname_content->width() / scale_value > 300) {  // max
			nickname_content->resize(297 * scale_value, 57 * scale_value);
		}
		else {  // 57 -> 宽度最大(scale_value最大时) change
			if (qint8(nickname_content->text().toStdString().length() - nickname_line_temp.toStdString().length()) >= 3)
				nickname_content->resize(scale_value * (57 + 14 * (nickname_content->text().toStdString().length() - 3)),
					57 * scale_value);
			else if (qint8(nickname_content->text().toStdString().length() - nickname_line_temp.toStdString().length()) > 0) {
				nickname_content->resize((nickname_content->width() / scale_value + 28) * scale_value,
					57 * scale_value);
			}
			else if (qint8(nickname_content->text().toStdString().length() - nickname_line_temp.toStdString().length()) <= -3) {
				nickname_content->resize((nickname_content->width() / scale_value - 42) * scale_value,
					57 * scale_value);
			}
			else {
				nickname_content->resize((nickname_content->width() / scale_value - 28) * scale_value,
					57 * scale_value);
			}
		}
		});
	connect(nickname_button, &QPushButton::clicked, this, [this]() -> void {
		nickname_flag = !nickname_flag;
		if (nickname_flag) {
			nickname_button->setStyleSheet("QPushButton{"
				"border-image: url(./1/button/pen_press.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/pen_press_float.png);}");
			nickname_button->setToolTip("点击确定昵称");
			nickname_content->show();
			std::thread rand_voice_thread(&Widget::testNicknameLineChanged, this);  // 开启类内线程
			rand_voice_thread.detach();  // 后台运行
		}
		else {
			nickname_button->setStyleSheet("QPushButton{"
				"border-image: url(./1/button/pen_common.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/pen_press_float.png);}");
			nickname_button->setToolTip("点击修改昵称");
			nickname_content->hide();
			QString his_user_name = user_name;
			if (nickname_content->text().isEmpty()) { nickname_content->setText(user_name); }
			else if (user_name == nickname_content->text()) {}
			else {
				user_name = nickname_content->text();
			}
		}
		});
	// 关闭按钮
	close_pet_button->move(320, 20);
	close_pet_button->resize(64, 64);
	close_pet_button->setStyleSheet("QPushButton{"
		"border-image: url(./1/button/power_common.png);}"
		"QPushButton:hover{"
		"border-image: url(./1/button/power_float.png);}");
	close_pet_button->setToolTip("下班收工");

	// ################################quit退出!!!#######################################
	connect(close_pet_button, &QPushButton::clicked, this, [&]() {
		close_flag = true;  // 关闭按钮为真
		touch_audio_player->stop();  // 关闭触摸语音资源
		dx_rand_voice = 0;  // 微分值清零中断线程
		rand_voice_flag = false;  // 关闭语音线程
		if (!nickname_flag || !user_name.isEmpty()) {  // 确认修改, 并且user不为空
			std::ofstream get_file_user_name("./user/user_name.txt");  // 关闭io保存昵称
			get_file_user_name << user_name.toStdString();  // 保存姓名
			get_file_user_name.close();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(DX * DX));  // 100ms关闭所有资源
		QCoreApplication::instance()->quit();  // 登出
		});

	// 敏感区域显示按钮
	paint_button = new QPushButton(this);
	paint_button->move(320, 84);
	paint_button->resize(64, 64);
	paint_button->setStyleSheet("QPushButton{"
		"border-image: url(./1/button/click.png);}"
		"QPushButton:hover{"
		"border-image: url(./1/button/click-float.png);}"
	);
	paint_button->setToolTip("观赏模式");
	connect(paint_button, &QPushButton::clicked, this, [&]() {
		sWidget::paint_version = !sWidget::paint_version;
		if (sWidget::paint_version) {
			paint_button->setStyleSheet("QPushButton{"
				"border-image: url(./1/button/common.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/common-float.png);}"
			);
			paint_button->setToolTip("敏感模式");
		}
		else {
			paint_button->setStyleSheet("QPushButton{"
				"border-image: url(./1/button/click.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/click-float.png);}"
			);
			paint_button->setToolTip("观赏模式");
		}
		});  // 点击按钮反转

	// 声音按钮
	his_rand = getRandValue(voice_dir_num);

	rand_voice_button->move(320, 148);
	rand_voice_button->resize(64, 64);
	rand_voice_button->setStyleSheet("QPushButton{"
		"border-image: url(./1/button/voice_press.png);}"
		"QPushButton:hover{"
		"border-image: url(./1/button/voice_press_float.png);}"
	);
	rand_voice_button->setToolTip("静音模式");
	connect(rand_voice_button, &QPushButton::clicked, this, [&]() {
		rand_voice_flag = !rand_voice_flag;
		if (rand_voice_flag) {
			rand_voice_button->setStyleSheet("QPushButton{"
				"border-image: url(./1/button/voice_common.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/voice_common_float.png);}"
			);
			rand_voice_next->show();
			rand_voice_pause->show();
			rand_voice_prev->show();
			rand_voice_slider->show();
			rand_voice_button->setToolTip("语音模式");
			std::thread rand_voice_thread(&Widget::randOpenVoice, this);  // 开启类内线程
			rand_voice_thread.detach();  // 后台运行
		}
		else {
			dx_rand_voice = 0;  // 中断语音
			rand_voice_next->hide();
			rand_voice_pause->hide();
			rand_voice_prev->hide();
			rand_voice_slider->hide();
			dialog_frame->setPlainText("当前无内容呦~");
			dialog_frame->resize(357 * scale_info[scale_variable], 43 * scale_info[scale_variable]);
			rand_voice_button->setStyleSheet("QPushButton{"
				"border-image: url(./1/button/voice_press.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/voice_press_float.png);}"
			);
			rand_voice_button->setToolTip("静音模式");
			rand_voice_pause->setStyleSheet("QPushButton{ "
				"border-image: url(./1/button/continue_common.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/continue_float.png);}");
		}
		});
	// 声音拓展按钮
	rand_voice_next = new QPushButton(this);
	rand_voice_next->move(384, 150);
	rand_voice_next->resize(20, 20);
	rand_voice_next->setStyleSheet("QPushButton{ "
		"border-image: url(./1/button/next_common.png);}"
		"QPushButton:hover{"
		"border-image: url(./1/button/next_float.png);}");
	connect(rand_voice_next, &QPushButton::clicked, this, [&]() -> void {
		if (his_rand < voice_dir_num && !rvoice_pause_flag)
			his_rand++, dx_rand_voice = 0;
		});

	rand_voice_pause = new QPushButton(this);
	rand_voice_pause->move(384, 170);
	rand_voice_pause->resize(20, 20);
	rand_voice_pause->setStyleSheet("QPushButton{ "
		"border-image: url(./1/button/continue_common.png);}"
		"QPushButton:hover{"
		"border-image: url(./1/button/continue_float.png);}");
	connect(rand_voice_pause, &QPushButton::clicked, this, [&]() {
		rvoice_pause_flag = !rvoice_pause_flag;
		if (rvoice_pause_flag) {  // 暂停
			dx_rand_voice = 0;  // 使线程中断音频播放,等待指令
			rand_voice_pause->setToolTip("暂停状态");
			rand_voice_pause->setStyleSheet("QPushButton{ "
				"border-image: url(./1/button/pause_common.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/pause_float.png);}");
		}
		else {  // 继续
			rand_voice_pause->setStyleSheet("QPushButton{ "
				"border-image: url(./1/button/continue_common.png);}"
				"QPushButton:hover{"
				"border-image: url(./1/button/continue_float.png);}");
			rand_voice_pause->setToolTip("播放状态");
		}
		});
	rand_voice_prev = new QPushButton(this);
	rand_voice_prev->move(384, 190);
	rand_voice_prev->resize(20, 20);
	rand_voice_prev->setStyleSheet("QPushButton{ "
		"border-image: url(./1/button/prev_common.png);}"
		"QPushButton:hover{"
		"border-image: url(./1/button/prev_float.png);}");
	connect(rand_voice_prev, &QPushButton::clicked, this, [&]() -> void {
		if (his_rand > 1 && !rvoice_pause_flag) his_rand--, dx_rand_voice = 0;
		});
	// 语音滑条音量
	rand_voice_player->setVolume(73);  // 设置播放器初始音量
	touch_audio_player->setVolume(73);
	rand_voice_slider->move(405, 150);
	rand_voice_slider->resize(20, 60);
	rand_voice_slider->setRange(0, 100);
	rand_voice_slider->setValue(73); // 设置初始值为 73
	rand_voice_slider->setStyleSheet("QSlider::handle:vertical  {"
		"min-width: 50px; min-height: 50px;"
		"image: url(./1/button/voice_slider_1.png);}"); // 调整滑块的位置
	connect(rand_voice_slider, &QSlider::valueChanged, [&](int value) {
		volume_value = value;
		rand_voice_player->setVolume(volume_value);
		touch_audio_player->setVolume(volume_value);
		});
	// 设置按钮提示
	rand_voice_next->setToolTip("下一句");
	rand_voice_pause->setToolTip("播放状态");
	rand_voice_prev->setToolTip("上一句");
	// 默认隐藏按钮
	rand_voice_next->hide();
	rand_voice_pause->hide();
	rand_voice_prev->hide();
	rand_voice_slider->hide();

	for (const auto& i : path_name) {  // 加载动画源
		show_gif_anime.push_back(new QMovie(i, QByteArray(), this));
	}

	show_gif_label->setMovie(show_gif_anime[INIT_PATH_ID]);
	show_gif_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	show_gif_label->setScaledContents(true);  // 自动适应

	std::for_each(show_gif_anime.begin(), show_gif_anime.end(), [&](QMovie* movie) {
		int index = std::distance(show_gif_anime.begin(),
		std::find(show_gif_anime.begin(), show_gif_anime.end(), movie));  // distance返回begin到目标的距离作为索引
	connect(movie, &QMovie::frameChanged, this, [=]() {
		if (path_id == index) {
			show_gif_label->setPixmap(QPixmap::fromImage(movie->currentImage().scaled(_GIF_WIDTH,
				_GIF_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation)));  // 如果是当前动画的连接则展示动画
			if (!judge_gif_loop[index]) {}
			else if (movie->currentFrameNumber() == movie->frameCount() - 1) {
				judge_loop_frequency++;
				if (judge_loop_frequency == judge_gif_loop[index]) {
					gif_over[index] = true;  // 播放完毕信号
					if (right_mouse) {
						showGif(history_catch_value - 1);
					}
					else showGif(INIT_GLOBAL_PATH_ID);  // 递归以退出循环, 使用默认动作
					judge_loop_frequency = 0;
				}
			}
		}
		});
		});

	showGif(path_id);  // 展示动画

	show_gif_label->resize(show_gif_label->size() * scale_info[scale_variable]);  // 设置桌宠大小

}

void Widget::showGif(qint8 path_id) {
	gif_over[Widget::path_id] = true;  // 先结束上次动画
	if (Widget::path_id != path_id || path_id == INIT_GLOBAL_PATH_ID) {  // 判断上一次的gif是否重复, 重复则不动, 否则改变
		if (Widget::path_id != INIT_GLOBAL_PATH_ID) show_gif_anime[Widget::path_id]->stop();
		Widget::path_id = path_id;
		//show_gif_label->setMovie(show_gif_anime[path_id]);
		show_gif_anime[path_id]->start();
		show_gif_label->show();
		judge_loop_frequency = 0;  // 意外更改退出动画,清零循环次数
	}
}

qint8 Widget::judgeDirection(QPoint& p1, QPoint& p2) {
	if (p1.x() - p2.x() == 0 && p1.y() - p2.y() == 0) return 5;
	else if (p1.x() - p2.x() <= 0 && p1.y() - p2.y() <= 0) return 1;
	else if (p1.x() - p2.x() <= 0 && p1.y() - p2.y() >= 0) return 2;
	else if (p1.x() - p2.x() >= 0 && p1.y() - p2.y() >= 0) return 3;
	else if (p1.x() - p2.x() >= 0 && p1.y() - p2.y() <= 0) return 4;
	return -1;
}

bool Widget::eventFilter(QObject* watched, QEvent* ev) {  // 鼠标事件重写
	QMouseEvent* mouse_ev = reinterpret_cast<QMouseEvent*>(ev);
	static QPoint start_pos;  // 起点

	static bool judge_drap_flag = !INIT_DRAP_TOOL;
	// 右键按下，记录坐标
	if (ev->type() == QEvent::MouseButtonPress && mouse_ev->button() & Qt::RightButton) {
		start_pos = mouse_ev->globalPos() - this->pos();  // 记录抓起坐标
	}
	// 移动, 右键按下
	else if (ev->type() == QEvent::MouseMove && mouse_ev->buttons() & Qt::MouseButton::RightButton) {
		right_mouse = true;
		QPoint mouse_position = mouse_ev->pos();
		qint8 _director = judgeDirection(mouse_position, history_position);
		//qDebug() << _director << "gif: " << gif_over[history_catch_value + 1];
		if (_director != 5 && gif_over[history_catch_value + 1]) {  // 只要在动并且动画播放完毕或者中断
			qint64 current_time = QDateTime::currentMSecsSinceEpoch();  // 获取当前时间戳
			qint64 delta_time = current_time - move_stun_timer;
			if (delta_time > 150) {   // 间隔时间过长300ms重新计入
				move_stun_timer = current_time;
				direction_change_counter = 0;
				jugde_direction = 0;
				history_position = mouse_position;
			}
			else {
				if (delta_time > 50) {  // 每50ms记录一次
					qreal delta_pos = sqrt((pow(mouse_position.x() - history_position.x(), 2)) +
						(pow(mouse_position.y() - history_position.y(), 2)));
					//qDebug() << delta_pos;
					if (jugde_direction == 0) {  // 没有方向则设置方向
						jugde_direction = _director;
					}
					else if (jugde_direction != _director) {  // 判断方向不一致
						//qDebug() << delta_pos / delta_time;
						if (delta_pos / delta_time > 0.4) {  // 判断速度大小
							direction_change_counter++;
							jugde_direction = _director;
						}
					}
					// 刷新点位和时间
					history_position = mouse_position;
					move_stun_timer = current_time;
					if (direction_change_counter > 6) {  // 晃6下
						//qDebug() << "晃动";
						gif_over[history_catch_value + 1] = false;  // 设置动画为播放中
						showGif(history_catch_value + 1);

						touch_audio_player->stop();  // 先使音频停止播放
						qint8 play_value = 3 * _TOUCH_AUDIO_NUMS
							- getRandValue(_TOUCH_AUDIO_NUMS);
						std::string voice_path = "./voice/touch_audio/" + std::to_string(play_value) + ".wav";
						touch_audio_player->setMedia(QUrl::fromLocalFile(QString::fromStdString(voice_path)));

						// 打开文件读取对话内容
						std::string text_path = "./voice/touch_audio/" + std::to_string(play_value) + ".txt";
						std::ifstream text_file(text_path);
						if (!text_file.is_open()) {
							qDebug() << "Error: Unable to open file ";
							rand_voice_text = "文本还未存在或者丢失~";
						}
						// 读取
						else while (std::getline(text_file, rand_voice_text));
						text_file.close();
						emit dialog_text_changed();  // 触发跟更改对话框信号

						touch_audio_player->play();  // 播放音频

						// 清零
						move_stun_timer = current_time;
						direction_change_counter = 0;
						jugde_direction = 0;
						history_position = mouse_position;
					}
				}

			}
		}
		//this->move(mouse_ev->globalPos() - start_pos);
		if (judge_drap_flag != drap_show_flag) {
			qint8 catch_rand_value = getRandValue(2);
			history_catch_value = catch_rand_value;
			if (catch_rand_value == 1) showGif(0);  // 根据随机值调用gif动画
			else if (catch_rand_value == 2) showGif(1);

			touch_audio_player->stop();  // 先使音频停止播放
			qint8 play_value = catch_rand_value * _TOUCH_AUDIO_NUMS
				- getRandValue(_TOUCH_AUDIO_NUMS);
			std::string voice_path = "./voice/touch_audio/" + std::to_string(play_value) + ".wav";
			touch_audio_player->setMedia(QUrl::fromLocalFile(QString::fromStdString(voice_path)));

			if (!rvoice_pause_flag && rand_voice_flag) {  // 如果没有暂停且随机语音开关也打开
				acc_interrupt = true;
				rvoice_pause_flag = true;  // 使其暂停
				rand_voice_pause->setStyleSheet("QPushButton{ "
					"border-image: url(./1/button/pause_common.png);}"
					"QPushButton:hover{"
					"border-image: url(./1/button/pause_float.png);}");
				dx_rand_voice = 0;  // 强行使线程中断音频播放,等待指令
			}

			// 打开文件读取对话内容
			std::string text_path = "./voice/touch_audio/" + std::to_string(play_value) + ".txt";
			std::ifstream text_file(text_path);
			if (!text_file.is_open()) {
				qDebug() << "Error: Unable to open file ";
				rand_voice_text = "文本还未存在或者丢失~";
			}
			// 读取
			else while (std::getline(text_file, rand_voice_text));
			text_file.close();
			emit dialog_text_changed();  // 触发跟更改对话框信号

			touch_audio_player->play();  // 播放音频

			judge_drap_flag = drap_show_flag;  // 锁定单一抓起姿势不刷新
		}
		if (history_catch_value == 1) {  // 笑
			this->move(mouse_ev->globalPos() - QPoint(this->width() * Ratio_<double, double>(41, 96) * scale_info[scale_variable],
				this->height() * Ratio_<double, double>(33, 192) * scale_info[scale_variable]));  // 设置移动坐标
		}
		else if (history_catch_value == 2) {  // 慌
			this->move(mouse_ev->globalPos() - QPoint(this->width() * Ratio_<double, double>(13, 36) * scale_info[scale_variable],
				this->height() * Ratio_<double, double>(9, 48) * scale_info[scale_variable]));  // 设置移动坐标
		}
	}
	else if (ev->type() == QEvent::MouseButtonPress && mouse_ev->button() == Qt::LeftButton) {  // 触摸判断
		qint8 s_cnt = INIT_MOVIE_NUM;  // 跳过默认和抓取
		for (auto& s_area : sensitive_area) {
			QRect convert_s_area = s_area;
			convert_s_area.setX(s_area.x() * scale_info[scale_variable]);
			convert_s_area.setY(s_area.y() * scale_info[scale_variable]);
			convert_s_area.setWidth(s_area.width() * scale_info[scale_variable]);
			convert_s_area.setHeight(s_area.height() * scale_info[scale_variable]);
			if (convert_s_area.contains(mouse_ev->pos())) {
				showGif(s_cnt);  // 触发动画

				// 触发音频
				touch_audio_player->stop();  // 先关闭先前音频
				qint16 play_value = s_cnt * _TOUCH_AUDIO_NUMS -
					getRandValue(_TOUCH_AUDIO_NUMS);
				std::string voice_path = "./voice/touch_audio/" + std::to_string(play_value) + ".wav";
				touch_audio_player->setMedia(QUrl::fromLocalFile(QString::fromStdString(voice_path)));

				// 打开文件读取对话内容
				std::string text_path = "./voice/touch_audio/" + std::to_string(play_value) + ".txt";
				std::ifstream text_file(text_path);
				if (!text_file.is_open()) {
					qDebug() << "Error: Unable to open file ";
					rand_voice_text = "文本还未存在或者丢失~";
				}
				// 读取
				else while (std::getline(text_file, rand_voice_text));
				text_file.close();

				emit dialog_text_changed();  // 触发跟更改对话框信号

				if (!rvoice_pause_flag && rand_voice_flag) {  // 如果没有暂停且随机语音开关也打开
					acc_interrupt = true;
					rvoice_pause_flag = true;  // 使其暂停
					rand_voice_pause->setStyleSheet("QPushButton{ "
						"border-image: url(./1/button/pause_common.png);}"
						"QPushButton:hover{"
						"border-image: url(./1/button/pause_float.png);}");
					dx_rand_voice = 0;  // 强行使线程中断音频播放,等待指令
				}
				touch_audio_player->play();
			}
			s_cnt++;
		}
	}
	else if (ev->type() == QEvent::MouseButtonRelease && mouse_ev->button()
		== Qt::RightButton) {  // mouse_ev->button() == Qt::RightButton 右键松开
		right_mouse = false;  // 松开
		judge_drap_flag = drap_show_flag;
		drap_show_flag = !drap_show_flag;
		drap_show_flag ? showAllModuel() : hideAllModuel();
		showGif(4);
	}

	return false;
}


void Widget::hideAllModuel() {
	paint_button->hide();
	close_pet_button->hide();
	rand_voice_button->hide();
	rand_voice_next->hide();
	rand_voice_pause->hide();
	rand_voice_prev->hide();
	rand_voice_slider->hide();
	dialog_frame->hide();
	nickname_button->hide();
	nickname_content->hide();
}  // 隐藏组件

void Widget::showAllModuel() {
	paint_button->show();
	close_pet_button->show();
	rand_voice_button->show();
	if (rand_voice_flag) {
		rand_voice_next->show();
		rand_voice_pause->show();
		rand_voice_prev->show();
		rand_voice_slider->show();
	}
	dialog_frame->show();
	nickname_button->show();
	if (nickname_flag)
		nickname_content->show();
} // 显示组件

void Widget::paintEvent(QPaintEvent* event) {

}

void Widget::wheelEvent(QWheelEvent* event) {  // 滚轮控制事件
	if (!right_mouse) {  // 没按右键情况下可缩放
		qint8 delta = event->angleDelta().y();
		unsigned char scale_history = scale_variable;  // 历史记录
		if (delta > 0) {
			scale_variable >= 5 ? scale_variable = 5 : scale_variable++;
			scaleWidgets(scale_history); // 放大
		}
		else {
			scale_variable <= 0 ? scale_variable = 0 : scale_variable--;
			scaleWidgets(scale_history); // 缩小
		}
	}
}

void Widget::scaleWidgets(unsigned char scale_history) {  // 缩放窗口
	QList<QWidget*> global_object = findChildren<QWidget*>();  // 获得所有的控件

	// 刷新动画
	show_gif_label->setVisible(false);
	for (const auto& _object : global_object) {  // 更改每一个控件的大小
		QSize newSize = _object->size() / scale_info[scale_history] * scale_info[scale_variable];
		_object->resize(newSize);
		_object->move(_object->pos() / scale_info[scale_history] * scale_info[scale_variable]);
	}
	std::string text_font_size = std::to_string(static_cast<int>(_DIALOG_FONT_SIZE * scale_info[scale_variable]));
	QString qss_text_content(QString::fromStdString("QTextEdit { font-size: " + text_font_size +
		"px; font-family: '大宝桃桃体'; color: rgb(255, 255, 255);"
		"    background: transparent;"
		"    border: 2px solid #c4dff6;"
		"    border-radius: 6px;"
		"    background-image: url(./1/background/dialog_frame.png);"
		"    background-repeat: no-repeat;"
		"    background-position: center;"
		"}"));
	std::string line_font_size = std::to_string(static_cast<int>(_LINE_FONT_SIZE * scale_info[scale_variable]));
	QString qss_line_content(QString::fromStdString("QLineEdit { font-size: " + line_font_size +
		"px; font-family: '大宝桃桃体'; "
		"    color: #456e7d; "
		"    background-color: #8db9ca;"
		"    border: 2px solid #c4dff6;"
		"    border-radius: 6px;"
		"}"));
	nickname_content->setStyleSheet(qss_line_content);
	dialog_frame->setStyleSheet(qss_text_content);
	update();  // 更新敏感区域显示
	show_gif_label->setVisible(true);

}

void Widget::testNicknameLineChanged() {  // 给昵称调整框发送缩放信号
	QString nickname_line_temp = nickname_content->text();
	while (nickname_flag) {
		if (close_flag) break;
		if (nickname_line_temp != nickname_content->text()) {
			emit nickname_line_changed(nickname_line_temp);
			nickname_line_temp = nickname_content->text();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(DX));
	}
}

void Widget::randOpenVoice() {  // 随机语音
	voice_mtx.lock();  // 上锁
	qint64 voice_duration = 0;  // 音频时间间隔
	bool stop_flag = false;  // 是否于播放完暂停
	static bool rand_message = false;
	static qint16 voice_player_num = 0;  // 语音序号
	if (rand_message != rand_voice_flag) {  // 如果为点击打开语音按钮则获取一次随机值
		his_rand = getRandValue(voice_dir_num);
		rand_message = rand_voice_flag;
	}
	while (rand_voice_flag) {
		voice_player_num = his_rand;
		// 没有暂停或者已经播放完内容则重新从磁盘加载
		if (dx_pause_value == 0) {
			std::string voice_path = "./voice/speak/" + std::to_string(voice_player_num) + ".wav";
			rand_voice_player->setMedia(QUrl::fromLocalFile(QString::fromStdString(voice_path)));
		}

		rand_voice_player->play();  // 播放
		volumeControlFadeIn(rand_voice_player);

		if ((dx_pause_value == 0 || acc_interrupt) && rand_voice_flag) {
			acc_interrupt = false;
			// 打开文件读取对话内容
			std::string text_path = "./voice/speak/" + std::to_string(voice_player_num) + ".txt";
			std::ifstream text_file(text_path);
			if (!text_file.is_open()) {
				qDebug() << "Error: Unable to open file ";
				rand_voice_text = "文本还未存在或者丢失~";
			}
			// 读取
			else while (std::getline(text_file, rand_voice_text));
			text_file.close();
			if (!rand_voice_flag) break;  // 磁盘io完检查一次线程是否关闭

			emit dialog_text_changed();  // 触发跟更改对话框信号
		}
		touch_audio_player->stop();  // 停止触摸音频
		if (dx_pause_value == 0)  // 只在获得音频的时候做duartion获取等待
			std::this_thread::sleep_for(std::chrono::milliseconds(DX * 5));
		// 时间间隔
		voice_duration = rand_voice_player->duration();
		dx_rand_voice = (voice_duration + RAND_VOICE_TIME_SLEEP) / DX;
		// dx 微分音频时长, 使分离的次线程及时退出
		for (qint64 i = dx_pause_value; i < dx_rand_voice; i++) {
			if (i == dx_rand_voice - 1) stop_flag = true;
			if (close_flag) break;
			dx_pause_value == 0 ? true : dx_pause_value = 0;  // 暂停值使用完后清零
			std::this_thread::sleep_for(std::chrono::milliseconds(DX));
			if (rvoice_pause_flag) {
				dx_pause_value = i;
				if (rand_voice_player->position() < voice_duration) {
					volumeControlFade(rand_voice_player);
					rand_voice_player->pause();  // 确定暂停则存储暂停值
				}
				else {
					volumeControlFade(rand_voice_player);
					rand_voice_player->stop();
					stop_flag = true;
				}
				break;
			}
		}
		while (rvoice_pause_flag && !close_flag && rand_voice_flag)
			std::this_thread::sleep_for(std::chrono::milliseconds(DX));  // 暂停线程
		if (stop_flag) {
			++voice_player_num;
			dx_pause_value = 0;  // 初始化语音起点值
			his_rand = voice_player_num;
			stop_flag = false;
		}  // 确定播放完,不然就是触发或者异常中断

		if (voice_player_num > voice_dir_num) his_rand =
			getRandValue(voice_dir_num);  // 如果播放完全部内容, 则再随机播放
	}
	rand_message = false;
	dx_pause_value = 0;
	rvoice_pause_flag = false;
	volumeControlFade(rand_voice_player);
	rand_voice_player->stop();  // stop彻底停止, pause暂停继续
	voice_mtx.unlock();
}

void Widget::volumeControlFadeIn(QMediaPlayer*& device) {
	for (int i = 0; i <= volume_value; i++) {
		;
		device->setVolume(i);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (close_flag) break;
	}
}// 淡入

void Widget::volumeControlFade(QMediaPlayer*& device) {
	for (int i = volume_value; i >= 0; i--) {
		device->setVolume(i);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (close_flag) break;
	}
}// 淡出

qint16 Widget::getRandValue(qint16 range) {
	std::random_device r_seed;
	std::mt19937 rand_tool(r_seed()); // Mersenne Twister初始化随机数生成器

	std::uniform_int_distribution<> random_value(1, range); // 生成1到range之间的随机整数

	return random_value(rand_tool);
}


Widget::~Widget()
{

}

qint16 count_files_in_directory(const std::string& path) {
	DIR* dir;                // 目录
	struct dirent* entry;    // dirent 结构体指针，用于表示目录中的条目
	int count = 0;

	// 打开指定路径的目录，并检查是否成功
	if ((dir = opendir(path.c_str())) != NULL) {
		// 遍历目录中的每个条目，直到结束
		while ((entry = readdir(dir)) != NULL) {
			// 如果条目是常规文件（DT_REG），则增加计数器
			if (entry->d_type == DT_REG) {
				count++;
			}
		}
		closedir(dir); // 关闭目录流
	}
	else {
		qDebug() << "Error opening directory";
	}

	return count / 2;
}
