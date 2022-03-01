# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/utils/imageProcs/tools/pymod/output.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
# Jason Albert - created 03/06/2014
# Python module to define common output functions

# This module provides a common interface to display messages
# to the screen.  Incorporated into those functions are the
# ability to log all messages and selectively output to the
# screen depending upon the verbosity level chosen.
# Also incorporated in this module is the ability to made
# API calls and capture the output from those calls for
# processing in the script

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
import os
import datetime
import logging
import ctypes
import io
import tempfile
import sys
import signal
import bz2
# Import builtins so we can call the actual python print function
# for output to the screen
# Without that, we get stuck in a loop of calling the module print
import builtins
from enum import Enum

# Define terminal colors
class tcolors(Enum):
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

    # Function to help with printing
    def __repr__(self):
        return str(self)

    def __str__(self):
        return self.value

    # Support string concat via + operations
    def __add__(self, other):
        return self.value + other

    def __radd__(self, other):
        return other + self.value

def colorStrip(line):
    '''
    Remove color codes from a line before it is logged to a file
    Any color code requires END to complete it
    To optimize for performance we then look for END
    If found, we then look for all supported codes to strip them out
    '''
    if (tcolors.END.value in line):
        for tcolor in tcolors:
            line = line.replace(tcolor.value, "")
    return line

############################################################
# Classes - Classes - Classes - Classes - Classes - Classes
############################################################
# These levels match the default logging levels, with the addition of one level
# That is the "BASE" level which is used to ensure the basic output to the screen
class LogLevels(Enum):
    DEBUG = 10
    INFO = 20
    WARN = 30
    ERROR = 40
    BASE = 45
    CRITICAL = 50

    # Function to help with printing and comparision
    def __repr__(self):
        return str(self)

    def __str__(self):
        return self.name

    def __int__(self):
        return self.value

    def __lt__(self, other):
        return self.value < other.value

    def __le__(self, other):
        return self.value <= other.value

    def __gt__(self, other):
        return self.value > other.value

    def __ge__(self, other):
        return self.value >= other.value

    # Need to make choices in argparse work
    def argparse(self):
        try:
            return LogLevels[self]
        except KeyError:
            return self

class out:
    def __init__(self, name="out"):
        self.name = name
        self.levels = LogLevels
        self.consoleLevel = self.levels.BASE
        self.logLevel = self.levels.INFO
        self.indent = 0
        self.indentStep = 2
        self.prefix = ""
        self.postfix = ""
        # The consuming program can optionally setup to log
        # By default, it is disabled
        self.log = None
        self.console = None
        # Default value for logOnly option on output calls
        # When None, nothing happens and should be this way 99% of the time
        # This is a master chicken switch to force the output function logOnly value
        self.logOnly = None

    ###########################
    # output functions
    ###########################
    def debug(self, *message, logOnly=False, doLevel=True):
        """
        Function to generate/print/log debug messages
        """
        return self.__output(self.levels.DEBUG, message, logOnly, doLevel)

    def info(self, *message, logOnly=False, doLevel=True):
        """
        Function to generate/print/log normal output printing
        """
        return self.__output(self.levels.INFO, message, logOnly, doLevel)

    def warning(self, *message, logOnly=False, doLevel=True):
        """
        Function to generate/print/log warning messages
        """
        return self.__output(self.levels.WARN, message, logOnly, doLevel)

    def error(self, *message, logOnly=False, doLevel=True):
        """
        Function to generate/print/log error messages
        """
        return self.__output(self.levels.ERROR, message, logOnly, doLevel)

    def print(self, *message, logOnly=False):
        """
        Function to generate/print/log standard messages that always go out
        """
        # print level messages never get a level prefix on the front
        return self.__output(self.levels.BASE, message, logOnly, doLevel=False)

    def critical(self, *message, logOnly=False, doLevel=True):
        """
        Function to generate/print/log critical messages
        """
        return self.__output(self.levels.CRITICAL, message, logOnly, doLevel)

    def __output(self, level, message, logOnly, doLevel):
        """
        Common function that underpins the debug/info/etc.. functions
        """
        if (self.logOnly):
            logOnly = self.logOnly
        # Store our different output type lines individually
        # Then we can call the output/log functions properly for each line
        logLines = list()
        consoleLines = list()
        # *args for message turns all input into a tuple
        # If it's a single arg tuple, break that out.  Otherwise process the tuple
        if (len(message) == 1):
            l_message = message[0]
        else:
            l_message = message

        # Handle multi-line messages that are output in one statement
        # This ensures everything gets leveled, prefixed and indented properly
        if isinstance(l_message, str):
            lines = l_message.split("\n")
        elif isinstance(l_message, list):
            lines = l_message
        elif isinstance(l_message, tuple):
            # Split tuple into pieces, turn each to string and join with space
            # This matches a 'normal' print of a tuple
            lines = [' '.join(map(lambda x: str(x), l_message))]
        else:
            lines = [str(l_message)]

        for line in lines:
            # Set up the base line we are going to log
            # It will then get tweaked as necessary for file or screen
            formatLine = self.prefix + (' ' * self.indent) + line + self.postfix

            # First - the normal log
            # This stays as is, just need to remove colors
            logLine = formatLine
            logLine = colorStrip(logLine)
            logLines.append(logLine)

            # Second - the console and console log
            # We need to store the console output 2 different ways
            # s = the output going to the screen - can have color codes
            # l = the output going to the log - can't have color codes
            consoleLine = dict()
            # Setup the screen output
            consoleLine['s'] = formatLine
            # If the level is to be on the message, find the start of the line and insert
            if (doLevel):
                idx = consoleLine['s'].find(line[0:20])
                consoleLine['s'] =  consoleLine['s'][:idx] + level.name + ": " +  consoleLine['s'][idx:]
            # Setup the log output
            consoleLine['l'] = consoleLine['s']
            consoleLine['l'] = colorStrip(consoleLine['s'])
            consoleLines.append(consoleLine)

        ####
        # Output the text
        ####
        # .log output
        if (self.log):
            for line in logLines:
                self.log.log(level.value, line)

        # Various console output types
        for line in consoleLines:
            if (self.console):
                self.console.log(level.value, line['l'])

            if (not logOnly):
                if (self.consoleLevel <= level):
                    builtins.print(line['s'])

    ###########################
    # setup/support functions
    ###########################
    def setIndent(self, num):
        """ 
        Sets the output indent on all printed lines
        """
        self.indent = num

    def getIndent(self):
        """
        Gets the output indent on all printed lines
        """
        return self.indent

    def moreIndent(self):
        """
        Increase the indent one level
        """
        self.indent += self.indentStep

    def lessIndent(self):
        """
        Decrease the indent one level
        """
        self.indent -= self.indentStep

    def setPrefix(self, prefix):
        """
        Set the prefix to put onto any printed lines
        Useful for programs scraping output looking for specific pattern
        """
        self.prefix = prefix

    def setPostfix(self, postfix):
        """
        Set the postfix to put onto any printed lines
        Useful for programs scraping output looking for specific pattern
        """
        self.postfix = postfix

    def date(self):
        """
        Return the current date/time
        """
        return datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    def getConsoleLevel(self):
        """
        Return the level we are currently at
        """
        return self.consoleLevel

    def setConsoleLevel(self, level):
        """
        Set the output level to the console
        If not called, will use the default defined in VarBox
        """
        self.consoleLevel = level

    def getLogLevel(self):
        """
        Return the level we are currently at
        """
        return self.logLevel

    def setLogLevel(self, level):
        """
        Set the output level to the log
        If not called, will use the default defined in VarBox
        """
        self.logLevel = level

    def setupLogging(self, filenameLog, filenameConsole):
        """
        Setup the logging infrastructure
        """
        # Load in custom levels
        for level in self.levels:
            logging.addLevelName(level.value, level.name)

        # Setup main log file
        if (filenameLog):
            if (filenameLog.endswith(".bz2")):
                logHandler = logging.StreamHandler(bz2.open(filenameLog, mode='wt', encoding='utf-8'))
            else:
                logHandler = logging.FileHandler(filenameLog, mode='w')
            logHandler.setFormatter(logging.Formatter("%(asctime)s %(levelname)-5s| %(message)s","%Y%m%d-%H%M%S"))

            self.log = logging.getLogger(self.name + "-log")
            self.log.addHandler(logHandler)
            self.log.setLevel(self.logLevel.value)

        # Setup console log file
        if (filenameConsole):
            if (filenameConsole.endswith(".bz2")):
                consoleHandler = logging.StreamHandler(bz2.open(filenameConsole, mode='wt', encoding='utf-8'))
            else:
                consoleHandler = logging.FileHandler(filenameConsole, mode='w')
            consoleHandler.setFormatter(logging.Formatter("%(message)s"))

            self.console = logging.getLogger(self.name + "-console")
            self.console.addHandler(consoleHandler)
            self.console.setLevel(self.consoleLevel.value)

    def shutdown(self):
        """
        Called at program exit to ensure anything in process gets flushed and closed
        """
        # Go through the log/console handlers and close them out
        # Especially important when writing directly to compressed logs.
        # They'll show as corrupted if not closed properly
        handlers = self.log.handlers[:]
        for handler in handlers:
            handler.close()
            self.log.removeHandler(handler)
        handlers = self.console.handlers[:]
        for handler in handlers:
            handler.close()
            self.console.removeHandler(handler)

    ###########################
    # capture functions
    ###########################

    # A program needs to be able to capture the stdout from function calls
    # in order to control what goes to the screen and ensure everything is
    # properly logged.  That is what this stdout_redirector function does
    # The concept/code comes from https://eli.thegreenplace.net/2015/redirecting-all-kinds-of-stdout-in-python/
    #
    # In the default implementation, if a ctrl-c is raised in the middle of this, we won't go
    # through the __exit__ call, which leaves stdout in a weird state
    # Output may not come out, we get errors about writing to closed files, etc..
    # To solve that problem we install our own signal handler for the length of the run
    # It just captures the ctrl-c happened and continues on until __exit__
    # Then, as the last thing in __exit__ it calls the original handler with the signal it caught
    # This ensures stdout gets set back properly!
    class stdout_redirector():
        def __init__(self, stream):
            self.stream = stream
            # Variables we need shared within the class
            self.original_stdout_fd = None
            self.saved_stdout_fd = None
            self.tfile = None
            # Variables for signal handling
            self.received_signal = False
            self.original_handler = None
            self.libc = ctypes.CDLL(None)
            self.c_stdout = ctypes.c_void_p.in_dll(self.libc, 'stdout')

        def __enter__(self):
            # Must be first: Install the signal handler
            self.original_handler = signal.signal(signal.SIGINT, self._handler)

            # The original fd stdout points to. Usually 1 on POSIX systems.
            self.original_stdout_fd = sys.stdout.fileno()

            # Save a copy of the original stdout fd in saved_stdout_fd
            self.saved_stdout_fd = os.dup(self.original_stdout_fd)

            # Create a temporary file and redirect stdout to it
            self.tfile = tempfile.TemporaryFile(mode='w+b')
            self._redirect_stdout(self.tfile.fileno())

        def __exit__(self, type, value, traceback):
            self._redirect_stdout(self.saved_stdout_fd)
            # Copy contents of temporary file to the given stream
            self.tfile.flush()
            self.tfile.seek(0, os.SEEK_SET)
            self.stream.write(self.tfile.read())
            self.tfile.close()
            os.close(self.saved_stdout_fd)

            # Must be last: Restore the orginal signal handler and call it if needed
            signal.signal(signal.SIGINT, self.original_handler)
            if (self.received_signal):
                self.original_handler(*self.received_signal)

        def _redirect_stdout(self, to_fd):
            """ Redirect stdout to the given file descriptor """
            # Flush the C-level buffer stdout
            self.libc.fflush(self.c_stdout)
            # Flush and close sys.stdout - also closes the file descriptor (fd)
            sys.stdout.close()
            # Make original_stdout_fd point to the same file as to_fd
            os.dup2(to_fd, self.original_stdout_fd)
            # Create a new sys.stdout that points to the redirected fd
            sys.stdout = io.TextIOWrapper(os.fdopen(self.original_stdout_fd, 'wb'), encoding='utf-8', line_buffering=True)
            #sys.stdout = codecs.getreader("utf-8")(os.fdopen(self.original_stdout_fd, 'wb'))

        def _handler(self, sig, frame):
            # Save the signal so we can put it out after we get everything cleaned up in __exit__
            self.received_signal = (sig, frame)
            # Debug tracing - since you can't print to the screen, increment size of file you can monitor
            #os.system("echo 1 >> /tmp/signal")

    def capture(self, function, *args, **kwargs):
        """
        Wraps the passed in function to collect any output to the screen
        Returns the rc from the function, along with the captured output
        """
        rc = 0
        msgs = list()
        f = io.BytesIO()
        hitException = False

        # Run our command capture the output
        # Do it within an exception handler to bolster the output capturing
        # Without the exception handler here, any of the captured output is lost
        # due to the fact it bails immediately and the msgs aren't processed
        # A future enhancement might be to rethrow the exception after output
        # is handled.  Not needed at this time though.
        try:
            with self.stdout_redirector(f):
                rc = function(*args, **kwargs)
        except KeyboardInterrupt:
            raise
        except BaseException as e:
            msgs.append("Exception - %s - %s" % (function, format(e)))
            hitException = True

        # Process the output captured and put it out
        for line in "{0}".format(f.getvalue().decode('utf-8')).splitlines():
            msgs.append(line)

        return (rc, msgs, hitException)
