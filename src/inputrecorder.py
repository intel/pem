#
# Copyright (c) 2014, Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#

"""
Records Keyboard and mouse events.
"""

from multiprocessing import Process, Queue
from Tkinter import Tk, Text, BOTH, W, N, E, S, END
from ttk import Frame, Button, Label, Style, Scrollbar
import datetime

from idlelib.WidgetRedirector import WidgetRedirector

from pem.keyboardbuffer import KeyboardBuffer



# pylint: disable=too-many-ancestors
# pylint: disable=too-many-public-methods

class _InputRecorder(Frame):
    """
    UI for recording input events.
    """

    class ReadOnlyText(Text):
        """
        Prevent keys from being shown in the text area.
        We want to control what is actualxpyl presented to the user.
        """
        def __init__(self, *args, **kwargs):
            """
            Trashes every attempt to modify the text area coming from
            user input.
            """
            Text.__init__(self, *args, **kwargs)
            self.redirector = WidgetRedirector(self)
            self.insert = \
                self.redirector.register("insert",
                                         lambda *args, **kw: "break")
            self.delete = \
                self.redirector.register("delete",
                                         lambda *args, **kw: "break")

    def __init__(self, parent, data_queue, handlers):
        Frame.__init__(self, parent)
        self._parent = parent
        self._start_time = 0
        self._data_queue = data_queue
        self._handlers = handlers
        self._style = None
        self._text_area = None
        self._init_ui()

    def _init_ui(self):
        """
        Composes the UI and associates the appropriate
        handler for the actions to catch.
        """
        self._parent.title("Events")
        self._style = Style()
        self._style.theme_use("default")
        self.pack(fill=BOTH, expand=1)

        self.columnconfigure(1, weight=1)
        self.columnconfigure(3, pad=7)
        self.rowconfigure(3, weight=1)
        self.rowconfigure(5, pad=7)

        label = Label(self, text="Actions")
        label.grid(sticky=W, pady=4, padx=5)

        self._text_area = self.ReadOnlyText(self)
        self._text_area.grid(row=1, column=0, columnspan=2, rowspan=4, padx=5,
                             sticky=E+W+S+N)
        scrollbar = Scrollbar(self, command=self._text_area.yview)
        scrollbar.grid(row=1, column=2, rowspan=4, sticky=N+S)
        self._text_area['yscrollcommand'] = scrollbar.set

        start_button = Button(self, text="Start", command=self._rec_start)
        start_button.grid(row=1, column=3)

        stop_button = Button(self, text="Stop", command=self._rec_stop)
        stop_button.grid(row=2, column=3, pady=4)

        save_button = Button(self, text="Save",)
        save_button.grid(row=5, column=3)

        quit_button = Button(self, text="Quit", command=self._exit)
        quit_button.grid(row=5, column=0, padx=5)

    def _elapsed_time_ms(self):
        """
        Returns the elapsed time in ms, relative to
        cls._start_time.
        """
        time = datetime.datetime.now() - self._start_time
        return (time.days * 24 * 60 * 60 + time.seconds) * 1000 + \
            time.microseconds / 1000

    def _key_press(self, event):
        """
        Local handler for intercepting key presses.
        """
        timestamp = self._elapsed_time_ms()
        KeyboardBuffer.press(event.keysym)
        buf = KeyboardBuffer.get_buffer()
        self._text_area.insert("end", "[{0}]    ".format(timestamp) +
                               "Pressed  {0}\t\t\tBuffer {1}\n"
                               .format(event.keysym, buf))
        self._text_area.see("end")
        self._data_queue.put(self._handlers["keyboard"](
            timestamp=timestamp, buf=buf, ))

    def _key_release(self, event):
        """
        Local handler for intercepting key releases.
        """
        timestamp = self._elapsed_time_ms()
        KeyboardBuffer.release(event.keysym)
        buf = KeyboardBuffer.get_buffer()
        self._text_area.insert("end", "[{0}]    ".format(timestamp) +
                               "Released {0}\t\t\tBuffer {1}\n"
                               .format(event.keysym, buf))
        self._text_area.see("end")
        self._data_queue.put(self._handlers["keyboard"](
            timestamp=timestamp, buf=buf, ))

    def _rec_start(self):
        """
        Start recording Keyboard and Mouse events.
        """
        self._text_area.delete(1.0, END)
        self._start_time = datetime.datetime.now()
        self._text_area.bind("<KeyPress>", self._key_press)
        self._text_area.bind("<KeyRelease>", self._key_release)
        self._data_queue.put(self._handlers["control"](command="Start"))

    def _rec_stop(self):
        """
        Stop recording Keyboard and Mouse events.
        """
        self._text_area.delete(1.0, END)
        self._text_area.unbind_all("<KeyPress>")
        self._text_area.unbind_all("<KeyRelease>")
        self._data_queue.put(self._handlers["control"](command="Stop"))

    def _exit(self):
        """
        Closes the program.
        """
        self._parent.destroy()
        self._data_queue.put(self._handlers["control"](command="Quit"))

    @staticmethod
    def build_ui(out_queue, handlers):
        """
        Draws the UI used for recording and starts the main loop.
        """
        root = Tk()
        _InputRecorder(parent=root, data_queue=out_queue,
                       handlers=handlers)
        root.mainloop()

# pylint: disableinvalid-name
_ui_process = None
# pylint: enableinvalid-name

def start_ui(handlers):
    """
    The entry point of the module:
    - takes a dictionary with formatters for various types of input data
    - returns a queue where it will push formatted data.
    """
    out_queue = Queue()
    # pylint: disable=invalid-name
    # pylint: disable=global-statement
    global _ui_process
    # pylint: enable=global-statement
    _ui_process = Process(target=_InputRecorder.build_ui,
                          args=(out_queue, handlers))
    _ui_process.start()
    # pylint: enable=invalid-name
    return out_queue

def wait_for_ui():
    """
    Thread Cleanup
    """
    if _ui_process is not None:
        _ui_process.join()
