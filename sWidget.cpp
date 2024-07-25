#include "sWidget.h"

bool sWidget::paint_version = false;  // 触摸的可视性

sWidget::sWidget(QWidget* parent = nullptr)
	: QWidget(parent)
{
	setWindowFlag(Qt::WindowType::FramelessWindowHint);
	// 透明背景
	setAttribute(Qt::WA_TranslucentBackground);
	// 显示在最上层
	setWindowFlag(Qt::WindowStaysOnTopHint);

	//QPen sensitive_pen(SENSITIVE_FRAME_RGB);
	//sensitive_pen.setWidth(2.5);
	//global_painter->setPen(sensitive_pen);

}


void sWidget::paintEvent(QPaintEvent* event) {
	if (paint_version) {
		QPainter ptemp(this);  // 敏感数组
		QPen sensitive_pen(SENSITIVE_FRAME_RGB);
		sensitive_pen.setWidth(2.5);
		ptemp.setPen(sensitive_pen);
		double& s_value = Widget::scale_info[Widget::scale_variable];  // 缩放量别名
		qint8 qarr_cnt = 0;
		for (const QRect& i : Widget::sensitive_area) {
			ptemp.drawRect(QRect(i.x() * s_value, i.y() * s_value,
				i.width() * s_value, i.height() * s_value));
		}
	}
}

void sWidget::wheelEvent(QWheelEvent* event) {

}

bool sWidget::eventFilter(QObject* wathced, QEvent* ev) {  // 鼠标事件重写

	return false;
}


sWidget::~sWidget()
{

}
