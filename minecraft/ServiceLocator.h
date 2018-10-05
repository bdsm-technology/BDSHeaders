#pragma once

template <typename T> class ServiceLocator {
  static T *mDefaultService;
  static T *mService;

public:
  static T &get();
  static bool isDefaultSet();
  static bool isSet();
  static void setDefault(T &);
};