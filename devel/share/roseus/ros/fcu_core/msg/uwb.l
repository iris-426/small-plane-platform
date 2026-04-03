;; Auto-generated. Do not edit!


(when (boundp 'fcu_core::uwb)
  (if (not (find-package "FCU_CORE"))
    (make-package "FCU_CORE"))
  (shadow 'uwb (find-package "FCU_CORE")))
(unless (find-package "FCU_CORE::UWB")
  (make-package "FCU_CORE::UWB"))

(in-package "ROS")
;;//! \htmlinclude uwb.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass fcu_core::uwb
  :super ros::object
  :slots (_header _tag1_id _tag2_id _distance ))

(defmethod fcu_core::uwb
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:tag1_id __tag1_id) 0)
    ((:tag2_id __tag2_id) 0)
    ((:distance __distance) 0.0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _tag1_id (round __tag1_id))
   (setq _tag2_id (round __tag2_id))
   (setq _distance (float __distance))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:tag1_id
   (&optional __tag1_id)
   (if __tag1_id (setq _tag1_id __tag1_id)) _tag1_id)
  (:tag2_id
   (&optional __tag2_id)
   (if __tag2_id (setq _tag2_id __tag2_id)) _tag2_id)
  (:distance
   (&optional __distance)
   (if __distance (setq _distance __distance)) _distance)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; int32 _tag1_id
    4
    ;; int32 _tag2_id
    4
    ;; float32 _distance
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; int32 _tag1_id
       (write-long _tag1_id s)
     ;; int32 _tag2_id
       (write-long _tag2_id s)
     ;; float32 _distance
       (sys::poke _distance (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; int32 _tag1_id
     (setq _tag1_id (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _tag2_id
     (setq _tag2_id (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; float32 _distance
     (setq _distance (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get fcu_core::uwb :md5sum-) "8fe3100dfde9957ab6550e380f2e7809")
(setf (get fcu_core::uwb :datatype-) "fcu_core/uwb")
(setf (get fcu_core::uwb :definition-)
      "std_msgs/Header header
int32 tag1_id
int32 tag2_id
float32 distance

================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
string frame_id

")



(provide :fcu_core/uwb "8fe3100dfde9957ab6550e380f2e7809")


