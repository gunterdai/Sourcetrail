#include "qt/view/QtViewOverlay.h"

#include <cmath>

#include <QEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

ResizeFilter::ResizeFilter(QWidget* widget)
	: m_widget(widget)
{
}

bool ResizeFilter::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		QWidget* parent = dynamic_cast<QWidget*>(obj);
		m_widget->setGeometry(0, 0, parent->width(), parent->height());
	}

	return QObject::eventFilter(obj, event);
}


QtOverlay::QtOverlay(QWidget* parent)
	: QWidget(parent)
	, m_count(0)
	, m_size(40)
{
}

void QtOverlay::animate()
{
	m_count += 25;

	QPoint center = geometry().center();
	update(QRect(center.x() - m_size / 2, center.y() - m_size / 2, m_size, m_size));
}

void QtOverlay::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	painter.fillRect(geometry(), "#AAFFFFFF");

	QPoint center = geometry().center();

	size_t x = m_size / 2;
	size_t y = m_size / 2;
	size_t s = (m_count / 7) % 200;

	if (s < 100)
	{
		x = std::cos(3.1415f * s / 100) * x;
	}
	else
	{
		y = std::cos(3.1415f * (s - 100) / 100) * y;
	}

	painter.translate(center.x(), center.y());
	// painter.rotate(m_count / 8);

	if (s > 50 && s < 150)
	{
		painter.setBrush(QBrush("#CC2E3C86"));
	}
	else
	{
		painter.setBrush(QBrush("#CC007AC2"));
	}

	painter.setPen(QPen(Qt::transparent));

	painter.drawEllipse(-x, -y, 2 * x, 2 * y);
}


QtViewOverlay::QtViewOverlay(QWidget* parent)
	: m_parent(parent)
	, m_beforeFunctor(std::bind(&QtViewOverlay::doBefore, this))
	, m_afterFunctor(std::bind(&QtViewOverlay::doAfter, this))
{
	m_overlay = new QtOverlay(m_parent);
	m_parent->installEventFilter(new ResizeFilter(m_overlay));
	m_overlay->hide();
	m_timer = new QTimer(m_overlay);
}

void QtViewOverlay::handleMessage(MessageLoadBefore* message)
{
	m_beforeFunctor();
}

void QtViewOverlay::handleMessage(MessageLoadAfter* message)
{
	m_afterFunctor();
}

void QtViewOverlay::doBefore()
{
	QObject::connect(m_timer, SIGNAL(timeout()), m_overlay, SLOT(animate()));

	m_timer->start(25);

	m_overlay->show();
}

void QtViewOverlay::doAfter()
{
	m_timer->stop();

	m_overlay->hide();
}