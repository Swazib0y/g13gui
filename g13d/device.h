#ifndef DEVICE_H
#define DEVICE_H

#include <libusb-1.0/libusb.h>
#include <linux/input.h>

#include <map>
#include <ostream>
#include <string>

#include "font.h"
#include "lcd.h"
#include "profile.h"
#include "stick.h"

namespace G13 {

class Manager;

class Device {
 public:
  Device(Manager &manager, libusb_device_handle *handle, int id);

  Manager &manager() {
    return _manager;
  }
  const Manager &manager() const {
    return _manager;
  }

  G13_LCD &lcd() {
    return _lcd;
  }
  const G13_LCD &lcd() const {
    return _lcd;
  }
  Stick &stick() {
    return _stick;
  }
  const Stick &stick() const {
    return _stick;
  }

  FontPtr switch_to_font(const std::string &name);
  void switch_to_profile(const std::string &name);
  ProfilePtr profile(const std::string &name);

  void dump(std::ostream &, int detail = 0);
  void command(char const *str);

  void read_commands();
  void read_config_file(const std::string &filename);

  int read_keys();
  void parse_joystick(unsigned char *buf);

  ActionPtr make_action(const std::string &);

  void set_key_color(int red, int green, int blue);
  void set_mode_leds(int leds);

  void send_event(int type, int code, int val);
  void write_output_pipe(const std::string &out);

  void write_lcd(unsigned char *data, size_t size);

  bool is_set(int key);
  bool update(int key, bool v);

  // used by Manager
  void cleanup();
  void register_context(libusb_context *ctx);
  void write_lcd_file(const std::string &filename);

  Font &current_font() {
    return *_current_font;
  }
  Profile &current_profile() {
    return *_current_profile;
  }

  int id_within_manager() const {
    return _id_within_manager;
  }

  typedef std::function<void(const char *)> COMMAND_FUNCTION;
  typedef std::map<std::string, COMMAND_FUNCTION> CommandFunctionTable;

 protected:
  void _init_fonts();
  void init_lcd();
  void _init_commands();

  // typedef void (COMMAND_FUNCTION)( Device*, const char *, const char * );
  CommandFunctionTable _command_table;

  struct timeval _event_time;
  struct input_event _event;

  int _id_within_manager;
  libusb_device_handle *handle;
  libusb_context *ctx;

  int _uinput_fid;

  int _input_pipe_fid;
  std::string _input_pipe_name;
  int _output_pipe_fid;
  std::string _output_pipe_name;

  std::map<std::string, FontPtr> _fonts;
  FontPtr _current_font;
  std::map<std::string, ProfilePtr> _profiles;
  ProfilePtr _current_profile;

  Manager &_manager;
  G13_LCD _lcd;
  Stick _stick;

  bool keys[G13_NUM_KEYS];
};

inline bool Device::is_set(int key) {
  return keys[key];
}

inline bool Device::update(int key, bool v) {
  bool old = keys[key];
  keys[key] = v;
  return old != v;
}

}  // namespace G13

#endif  // DEVICE_H
