
(cl:in-package :asdf)

(defsystem "fcu_core-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "uwb" :depends-on ("_package_uwb"))
    (:file "_package_uwb" :depends-on ("_package"))
    (:file "uwbs" :depends-on ("_package_uwbs"))
    (:file "_package_uwbs" :depends-on ("_package"))
  ))