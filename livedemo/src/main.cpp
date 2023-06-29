#include <iostream>

#include <patrick/core.h>

#include <livedemo/channel.h>
#include <livedemo/command.h>

using namespace livedemo;

int
main ()
{
  command_line cmdline{ "Greetings!\n" };

  cmdline.add_cmd ("set_channel", commands::set_channel);
  cmdline.add_cmd ("transfer", commands::transfer_through_channel);
  cmdline.add_cmd ("load", commands::load_code);
  cmdline.add_cmd ("unload", commands::unload_code);
  cmdline.add_cmd ("encode", commands::encode);
  cmdline.add_cmd ("decode", commands::decode);
  cmdline.add_cmd ("props", commands::props);
  cmdline.add_cmd ("show_table", commands::show_slepian_table);
  cmdline.add_cmd ("show_codewords", commands::show_codewords);
  cmdline.add_cmd ("show_generator", commands::show_generator);
  cmdline.add_cmd ("show_parity", commands::show_parity);
  cmdline.add_cmd ("exit", commands::exit);
  cmdline.add_cmd ("help", commands::help);

  cmdline.loop ();

  return 0;
}
