; Auto-generated. Do not edit!


(cl:in-package fcu_core-msg)


;//! \htmlinclude uwb.msg.html

(cl:defclass <uwb> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (tag1_id
    :reader tag1_id
    :initarg :tag1_id
    :type cl:integer
    :initform 0)
   (tag2_id
    :reader tag2_id
    :initarg :tag2_id
    :type cl:integer
    :initform 0)
   (distance
    :reader distance
    :initarg :distance
    :type cl:float
    :initform 0.0))
)

(cl:defclass uwb (<uwb>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <uwb>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'uwb)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name fcu_core-msg:<uwb> is deprecated: use fcu_core-msg:uwb instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <uwb>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader fcu_core-msg:header-val is deprecated.  Use fcu_core-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'tag1_id-val :lambda-list '(m))
(cl:defmethod tag1_id-val ((m <uwb>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader fcu_core-msg:tag1_id-val is deprecated.  Use fcu_core-msg:tag1_id instead.")
  (tag1_id m))

(cl:ensure-generic-function 'tag2_id-val :lambda-list '(m))
(cl:defmethod tag2_id-val ((m <uwb>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader fcu_core-msg:tag2_id-val is deprecated.  Use fcu_core-msg:tag2_id instead.")
  (tag2_id m))

(cl:ensure-generic-function 'distance-val :lambda-list '(m))
(cl:defmethod distance-val ((m <uwb>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader fcu_core-msg:distance-val is deprecated.  Use fcu_core-msg:distance instead.")
  (distance m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <uwb>) ostream)
  "Serializes a message object of type '<uwb>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:let* ((signed (cl:slot-value msg 'tag1_id)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'tag2_id)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'distance))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <uwb>) istream)
  "Deserializes a message object of type '<uwb>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'tag1_id) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'tag2_id) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'distance) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<uwb>)))
  "Returns string type for a message object of type '<uwb>"
  "fcu_core/uwb")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'uwb)))
  "Returns string type for a message object of type 'uwb"
  "fcu_core/uwb")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<uwb>)))
  "Returns md5sum for a message object of type '<uwb>"
  "8fe3100dfde9957ab6550e380f2e7809")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'uwb)))
  "Returns md5sum for a message object of type 'uwb"
  "8fe3100dfde9957ab6550e380f2e7809")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<uwb>)))
  "Returns full string definition for message of type '<uwb>"
  (cl:format cl:nil "std_msgs/Header header~%int32 tag1_id~%int32 tag2_id~%float32 distance~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'uwb)))
  "Returns full string definition for message of type 'uwb"
  (cl:format cl:nil "std_msgs/Header header~%int32 tag1_id~%int32 tag2_id~%float32 distance~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <uwb>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <uwb>))
  "Converts a ROS message object to a list"
  (cl:list 'uwb
    (cl:cons ':header (header msg))
    (cl:cons ':tag1_id (tag1_id msg))
    (cl:cons ':tag2_id (tag2_id msg))
    (cl:cons ':distance (distance msg))
))
