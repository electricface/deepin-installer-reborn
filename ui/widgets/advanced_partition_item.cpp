// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "ui/widgets/advanced_partition_item.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

#include "partman/partition_manager.h"
#include "ui/frames/delegates/partition_util.h"
#include "ui/widgets/flat_button.h"
#include "ui/widgets/partition_usage_bar.h"

namespace ui {

AdvancedPartitionItem::AdvancedPartitionItem(
    const partman::Partition& partition, QWidget* parent)
    : QFrame(parent),
      partition_(partition),
      selected_(false) {
  this->setObjectName(QStringLiteral("advanced_partition_item"));

  this->initUI();
  this->initConnections();
}

void AdvancedPartitionItem::setEditable(bool editable) {
  if (editable) {
    if (partition_.type == partman::PartitionType::Unallocated ||
        partition_.type == partman::PartitionType::LogicalUnallocated) {
      control_status_ = ControlStatus::New;
      control_button_->setIcon(
          QIcon(QStringLiteral(":/images/new_partition.png")));
    } else {
      control_status_ = ControlStatus::Edit;
      control_button_->setIcon(
          QIcon(QStringLiteral(":/images/edit_partition.png")));
    }
  } else {
    if (selected_) {
      control_status_ = ControlStatus::Delete;
      control_button_->setIcon(
          QIcon(QStringLiteral(":/images/delete_partition.png")));
    } else {
      control_status_ = ControlStatus::Hide;
    }
  }

  control_button_->setVisible(control_status_ != ControlStatus::Hide);
}

void AdvancedPartitionItem::setMountPoint(const QString& mount_point) {
  mount_point_label_->setText(mount_point);
}

void AdvancedPartitionItem::setFilesystemType(const QString& fs) {
  fs_type_label_->setText(fs);
}

void AdvancedPartitionItem::initConnections() {
  connect(control_button_, &QPushButton::clicked,
          this, &AdvancedPartitionItem::onControlButtonClicked);
}

void AdvancedPartitionItem::initUI() {
  // os-prober logo
  // filesystem type
  // partition label
  partition_label_ = new QLabel();
  if (!partition_.label.isEmpty()) {
    partition_label_->setText(partition_.label);
  } else {
    partition_label_->setText(GetPartitionName(partition_.path));
  }
  partition_label_->setObjectName(QStringLiteral("partition_label"));

  partition_path_label_ = new QLabel();
  partition_path_label_->setText(
      QString("(%1)").arg(GetPartitionName(partition_.path)));
  partition_path_label_->setObjectName(QStringLiteral("partition_path_label"));

  // partition space usage
  usage_label_ = new QLabel();
  usage_label_->setText(GetPartitionUsage(partition_.freespace,
                                          partition_.length));
  usage_label_->setObjectName(QStringLiteral("usage_label"));
  PartitionUsageBar* usage_bar = new PartitionUsageBar(partition_.freespace,
                                                       partition_.length);

  // mount point
  mount_point_label_ = new QLabel();
  mount_point_label_->setObjectName(QStringLiteral("mount_point_label"));

  // tip
  tip_label_ = new QLabel();
  tip_label_->setObjectName(QStringLiteral("tip_label"));

  // filesystem name
  fs_type_label_ = new QLabel();
  fs_type_label_->setText(GetFsTypeName(partition_.fs));
  fs_type_label_->setObjectName(QStringLiteral("fs_type_label"));

  control_button_ = new FlatButton();
  control_button_->setFixedSize(18, 18);
  control_button_->hide();

  // TODO(xuhaohua): Use fixed layout instead.
  QHBoxLayout* layout = new QHBoxLayout();
  layout->addWidget(partition_label_);
  layout->addWidget(partition_path_label_);
  layout->addStretch();
  layout->addWidget(usage_label_);
  layout->addWidget(usage_bar);
  layout->addStretch();
  layout->addWidget(mount_point_label_);
  layout->addStretch();
  layout->addWidget(tip_label_);
  layout->addStretch();
  layout->addWidget(fs_type_label_);
  layout->addStretch();
  layout->addWidget(control_button_);

  this->setLayout(layout);

  this->setFixedSize(480, 36);
}

void AdvancedPartitionItem::onControlButtonClicked() {
  switch (control_status_) {
    case ControlStatus::Delete: {
      emit this->deletePartitionTriggered(partition_.path);
      break;
    }
    case ControlStatus::Edit: {
      emit this->editPartitionTriggered(partition_.path);
      break;
    }
    case ControlStatus::New: {
      emit this->newPartitionTriggered(partition_.path);
      break;
    }
    default: {
      // Never reach here.
      break;
    }
  }
}

}  // namespace ui