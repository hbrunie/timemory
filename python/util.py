#!/usr/bin/env python

# MIT License
#
# Copyright (c) 2018, The Regents of the University of California,
# through Lawrence Berkeley National Laboratory (subject to receipt of any
# required approvals from the U.S. Dept. of Energy).  All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

from __future__ import absolute_import

import os
import sys

#sys.path.append(os.path.abspath(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))))

#------------------------------------------------------------------------------#
class auto_timer(object):
    """ A decorator for the auto-timer, e.g.:
        @timemory.util.auto_timer("'AUTO_TIMER_DECORATOR_KEY_TEST':{}".format(tim.LINE()))
        def main(n):
            for i in range(2):
                fibonacci(n * (i+1))
        # ...
    """
    def __init__(self, key="", add_args=False, is_class=False):
        self.key = key
        self.add_args = add_args
        self.is_class = is_class


    def __call__(self, func):

        import timemory as tim
        from functools import wraps
        _file = tim.FILE(3)
        _line = tim.LINE(2)

        @wraps(func)
        def function_wrapper(*args, **kwargs):

            # add_args only if key not specified
            if self.add_args and self.key == "":
                self.key = self.arg_string(args, kwargs)
            else:
                self.key = '{}{}'.format(self.key, self.class_string(args, kwargs))

            t = tim.timer_decorator(func.__name__, _file, _line,
                self.key, self.add_args)

            return func(*args, **kwargs)

        return function_wrapper


    # ------------------------------------------------------------------------ #
    def class_string(self, args, kwargs):
        """
        Generate a class identifier
        """
        _str = ''
        if self.is_class and len(args) > 0:
            _str = '[{}]'.format(type(args[0]).__name__)
        return _str


    # ------------------------------------------------------------------------ #
    def arg_string(self, args, kwargs):
        """
        Generate a string of the arguments
        """
        _str = '{}('.format(self.class_string(args, kwargs))
        for i in range(0, len(args)):
            if i == 0:
                _str = '{}{}'.format(_str, args[i])
            else:
                _str = '{}, {}'.format(_str, args[i])

        for key, val in kwargs:
            _str = '{}, {}={}'.format(_str, key, val)

        return '{})'.format(_str)


#------------------------------------------------------------------------------#
class timer(object):
    """ A decorator for the timer, e.g.:

        class someclass(object):

            @timemory.util.timer(is_class=True)
            def __init__(self):
                self.some_obj = None
        # ...
    """
    def __init__(self, key="", add_args=False, is_class=False, nback=2):
        self.key = key
        self.add_args = add_args
        self.is_class = is_class


    def __call__(self, func):

        import timemory as tim
        from functools import wraps
        _file = tim.FILE(3)
        _line = tim.LINE(2)

        @wraps(func)
        def function_wrapper(*args, **kwargs):

            t = None
            if self.key == "":
                _func = func.__name__
                self.key = '{}{}'.format(self.key, self.class_string(args, kwargs))
                # add_args only if key not specified
                if self.add_args:
                    self.key = self.arg_string(args, kwargs)
                t = tim.timer('{}{}@{}:{}'.format(_func, self.key, _file, _line))
            else:
                self.key = '{}{}'.format(self.key, self.class_string(args, kwargs))
                t = tim.timer(self.key)

            t.start()
            ret = func(*args, **kwargs)
            t.stop()
            t.report()
            return ret

        return function_wrapper

    # ------------------------------------------------------------------------ #
    def class_string(self, args, kwargs):
        """
        Generate a class identifier
        """
        _str = ''
        if self.is_class and len(args) > 0:
            _str = '[{}]'.format(type(args[0]).__name__)
        return _str


    # ------------------------------------------------------------------------ #
    def arg_string(self, args, kwargs):
        """
        Generate a string of the arguments
        """
        _str = '{}('.format(self.class_string(args, kwargs))
        for i in range(0, len(args)):
            if i == 0:
                _str = '{}{}'.format(_str, args[i])
            else:
                _str = '{}, {}'.format(_str, args[i])

        for key, val in kwargs:
            _str = '{}, {}={}'.format(_str, key, val)

        return '{})'.format(_str)