#pragma once

#include <QWidget>
#include "Widget.h"

class sWidget : public QWidget
{
	Q_OBJECT

public:
	sWidget(QWidget* parent);
	~sWidget();

public:
	void paintEvent(QPaintEvent* event) override;  // 重写绘制事件
	void wheelEvent(QWheelEvent* event) override;  // 重写滚轮事件
	bool eventFilter(QObject* wathced, QEvent* ev) override;  // 鼠标事件

private:

public:
	static bool paint_version;  // 判断按钮

};
