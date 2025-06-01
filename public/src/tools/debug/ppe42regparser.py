class MSRRegister:
    def __init__(self, value):
        self.value = value & 0xFFFFFFFF  # Ensure 32-bit

    def _get_bits(self, start, end=None):
        if end is None:
            end = start
        mask = (1 << (end - start + 1)) - 1
        return (self.value >> start) & mask

    def _fields(self):
        return [
            ((0,),     'RESV',   'Reserved. Must be written as 0.'),
            ((1, 7),   'SEM',    'SIB Error Mask. Selects which SIB return codes cause errors.'),
            ((8,),     'IS0',    'Instance-specific control bit (implementation-defined).'),
            ((9, 11),  'SIBRC',  'Last SIB Return Code from memory interface.'),
            ((12,),    'LP',     'Low Priority Mode. Reduces memory priority when set.'),
            ((13,),    'WE',     'WAIT Enable. Enables WAIT instruction behavior.'),
            ((14,),    'IS1',    'Instance-specific control bit (implementation-defined).'),
            ((15,),    'UIE',    'Unmaskable Interrupt Enable. Enables unmaskable exceptions.'),
            ((16,),    'EE',     'External Interrupt Enable. Enables asynchronous interrupts.'),
            ((19,),    'ME',     'Machine Check Enable. Enables machine check interrupts.'),
            ((20,),    'IS2',    'Instance-specific control bit (implementation-defined).'),
            ((21,),    'IS3',    'Instance-specific control bit (implementation-defined).'),
            ((23,),    'IPE',    'Imprecise Mode Enable. Allows relaxed store timing.'),
            ((24, 31), 'SIBRCA', 'SIB Return Code Accumulator. Aggregates SIB error results.'),
        ]

    def _symbolic(self, field, val):
        return {
            'LP':   {0: 'Normal', 1: 'Low priority'},
            'WE':   {0: 'WAIT disabled', 1: 'WAIT enabled'},
            'UIE':  {0: 'Masked', 1: 'Unmasked'},
            'EE':   {0: 'Disabled', 1: 'Enabled'},
            'ME':   {0: 'Disabled', 1: 'Enabled'},
            'IPE':  {0: 'Precise mode', 1: 'Imprecise mode'},
        }.get(field, {}).get(val, '')

    def print_table(self):
        # Register header
        print("Register: Machine State Register (MSR)")
        print(f"Value    : 0x{self.value:08X} / {format(self.value, '032b')}\n")

        # Column headers
        header = f"{'Bits':<9} {'Field':<8} {'Description':<60} {'Hex':<6} {'Binary':<10} {'Interpretation'}"
        print(header)
        print("-" * len(header))

        for (start, *maybe_end), field, desc in self._fields():
            end = maybe_end[0] if maybe_end else start
            bit_range = f"{start}" if start == end else f"{start}:{end}"
            val = self._get_bits(start, end)
            width = end - start + 1
            val_bin = format(val, f"0{width}b")
            val_hex = f"{val:X}"
            interp = self._symbolic(field, val)
            print(f"{bit_range:<9} {field:<8} {desc:<60} {val_hex:<6} {val_bin:<10} {interp}")

class XCRRegister:
    def __init__(self, value):
        self.value = value & 0xFFFFFFFF  # Ensure 32-bit

    def _get_bits(self, start, end=None):
        if end is None:
            end = start
        mask = (1 << (end - start + 1)) - 1
        return (self.value >> start) & mask

    def _fields(self):
        return [
            ((0,),     'RESV0', 'Reserved. Always write as 0.'),
            ((1, 3),   'CMD',   'Command sent to processor via XCR.'),
            ((4, 31),  'RESV1', 'Reserved. Must be 0.'),
        ]

    def _symbolic(self, field, val):
        if field == 'CMD':
            return {
                0b000: 'Clear Debug Status',
                0b001: 'Halt',
                0b010: 'Resume',
                0b011: 'Single-step',
                0b100: 'Toggle Timers (XSR[TRH])',
                0b101: 'Soft Reset',
                0b110: 'Hard Reset',
                0b111: 'Force Halt',
            }.get(val, 'Unknown command')
        return ''

    def print_table(self):
        print("Register: External Control Register (XCR)")
        print(f"Value    : 0x{self.value:08X} / {format(self.value, '032b')}\n")

        header = f"{'Bits':<9} {'Field':<8} {'Description':<60} {'Hex':<6} {'Binary':<10} {'Interpretation'}"
        print(header)
        print("-" * len(header))

        for (start, *maybe_end), field, desc in self._fields():
            end = maybe_end[0] if maybe_end else start
            bit_range = f"{start}" if start == end else f"{start}:{end}"
            val = self._get_bits(start, end)
            width = end - start + 1
            val_bin = format(val, f"0{width}b")
            val_hex = f"{val:X}"
            interp = self._symbolic(field, val)
            print(f"{bit_range:<9} {field:<8} {desc:<60} {val_hex:<6} {val_bin:<10} {interp}")

class XSRRegister:
    def __init__(self, value):
        self.value = value & 0xFFFFFFFF

    def _get_bits(self, start, end=None):
        if end is None:
            end = start
        mask = (1 << (end - start + 1)) - 1
        return (self.value >> start) & mask

    def _fields(self):
        return [
            ((0,),     'HS',     'Halted State. 1 = Processor is halted.'),
            ((1, 3),   'HC',     'Halt Condition. Indicates the cause of the halt.'),
            ((4,),     'HCP',    'Halt Condition Present. 1 = A halt condition is active.'),
            ((5,),     'RIP',    'Ramming In Progress. 1 = RAM execution is in progress.'),
            ((6,),     'SIP',    'Single-step In Progress. 1 = single-step is in progress.'),
            ((7,),     'TRAP',   'Trap debug event occurred.'),
            ((8,),     'IAC',    'Instruction Address Compare match occurred.'),
            ((9, 11),  'SIBRC',  'Last SIB Return Code.'),
            ((12,),    'RDAC',   'Read Data Address Compare debug event occurred.'),
            ((13,),    'WDAC',   'Write Data Address Compare debug event occurred.'),
            ((14,),    'WS',     'Wait State. Reflects MSR[WE].'),
            ((15,),    'TRH',    'Timers Run While Halted. 1 = timers run while halted.'),
            ((16, 19), 'SMS',    'State Machine State.'),
            ((20,),    'LP',     'Low Priority. Reflects MSR[LP].'),
            ((21,),    'EP',     'Maskable Event Pending. Reflects ISR[EP].'),
            ((22,),    'EE',     'External Enable. Reflects MSR[EE] (PPE42X only).'),
            ((23,),    'RESV1',  'Reserved.'),
            ((24,),    'PTR',    'Program Trap. Reflects ISR[PTR].'),
            ((25,),    'ST',     'Store Type Interrupt. Reflects ISR[ST].'),
            ((26, 27), 'RESV2',  'Reserved.'),
            ((28,),    'MFE',    'Multiple Fault Error. Reflects ISR[MFE].'),
            ((29, 31), 'MCS',    'Machine Check Status. Reflects ISR[MCS].'),
        ]

    def _symbolic(self, field, val):
        if field == 'HC':
            return {
                0b000: 'None',
                0b001: 'Force Halt',
                0b010: 'WDT Halt',
                0b011: 'Unmaskable Halt',
                0b100: 'Debug Halt',
                0b101: 'DBCR Halt',
                0b110: 'Ext Halt',
                0b111: 'Hardware Failure',
            }.get(val, '')
        if field == 'HS':
            return '1 = processor is halted.' if val else ''
        if field == 'HCP':
            return '1 = a halt condition is active.' if val else ''
        if field == 'TRH':
            return '1 = timers run while halted.' if val else '0 = timers are frozen while halted.'
        if field == 'LP':
            return '1 = low priority mode active.' if val else '0 = normal priority mode.'
        if field == 'EE':
            return '1 = external interrupts enabled.' if val else '0 = external interrupts disabled.'
        return ''

    def print_table(self):
        print("Register: External Status Register (XSR)")
        print(f"Value    : 0x{self.value:08X} / {format(self.value, '032b')}\n")

        header = f"{'Bits':<9} {'Field':<8} {'Description':<60} {'Hex':<6} {'Binary':<10} {'Interpretation'}"
        print(header)
        print("-" * len(header))

        for (start, *maybe_end), field, desc in self._fields():
            end = maybe_end[0] if maybe_end else start
            bit_range = f"{start}" if start == end else f"{start}:{end}"
            val = self._get_bits(start, end)
            width = end - start + 1
            val_bin = format(val, f"0{width}b")
            val_hex = f"{val:X}"
            interp = self._symbolic(field, val)
            print(f"{bit_range:<9} {field:<8} {desc:<60} {val_hex:<6} {val_bin:<10} {interp}")

import textwrap

class XERRegister:
    def __init__(self, value):
        self.value = value & 0xFFFFFFFF

    def _get_bits(self, start, end=None):
        if end is None:
            end = start
        mask = (1 << (end - start + 1)) - 1
        return (self.value >> start) & mask

    def _fields(self):
        return [
            ((0,),     'SO',    'Summary Overflow. 0 = No overflow has occurred. 1 = Overflow has occurred.'),
            ((1,),     'OV',    'Overflow. 0 = No overflow has occurred. 1 = Overflow has occurred.'),
            ((2,),     'CA',    'Carry. 0 = Carry has not occurred. 1 = Carry has occurred.'),
            ((3, 31),  'RESV',  'Reserved.'),
        ]

    def _symbolic(self, field, val):
        if field == 'SO':
            return '1 = Overflow has occurred.' if val else '0 = No overflow has occurred.'
        if field == 'OV':
            return '1 = Overflow has occurred.' if val else '0 = No overflow has occurred.'
        if field == 'CA':
            return '1 = Carry has occurred.' if val else '0 = Carry has not occurred.'
        return ''

    def print_table(self):
        print("Register: Fixed-Point Exception Register (XER)")
        print(f"Value    : 0x{self.value:08X} / {format(self.value, '032b')}\n")

        col_widths = {
            'Bits': 9,
            'Field': 8,
            'Hex': 6,
            'Binary': 10,
            'DescWrap': 65,
        }

        header = f"{'Bits':<{col_widths['Bits']}} {'Field':<{col_widths['Field']}} {'Description':<{col_widths['DescWrap']}} {'Hex':<{col_widths['Hex']}} {'Binary':<{col_widths['Binary']}} {'Interpretation'}"
        print(header)
        print("-" * len(header))

        for (start, *maybe_end), field, desc in self._fields():
            end = maybe_end[0] if maybe_end else start
            bit_range = f"{start}" if start == end else f"{start}:{end}"

            val = self._get_bits(start, end)
            width = end - start + 1
            val_bin = format(val, f"0{width}b")
            val_hex = f"{val:X}"
            interp = self._symbolic(field, val)

            # Wrap long descriptions
            wrapped_desc = textwrap.wrap(desc, width=col_widths['DescWrap'])
            for i, line in enumerate(wrapped_desc):
                if i == 0:
                    print(f"{bit_range:<{col_widths['Bits']}} {field:<{col_widths['Field']}} {line:<{col_widths['DescWrap']}} {val_hex:<{col_widths['Hex']}} {val_bin:<{col_widths['Binary']}} {interp}")
                else:
                    print(f"{'':<{col_widths['Bits']}} {'':<{col_widths['Field']}} {line:<{col_widths['DescWrap']}}")


import textwrap

class TSRRegister:
    def __init__(self, value):
        self.value = value & 0xFFFFFFFF

    def _get_bits(self, start, end=None):
        if end is None:
            end = start
        mask = (1 << (end - start + 1)) - 1
        return (self.value >> start) & mask

    def _fields(self):
        return [
            ((0,),     'ENW',  "Enable Next Watchdog. If '0', the watchdog event sets ENW = '1'. If '1', the event triggers WDT interrupt or reset."),
            ((1,),     'WIS',  "Watchdog Timer Interrupt Status. 1 = A watchdog timer event has occurred."),
            ((2,3),    'WRS',  "Watchdog Timer Reset Status. Indicates last WDT action: 00 = none, 01 = soft reset, 10 = hard reset, 11 = halt."),
            ((4,),     'DIS',  "Decrementer Interrupt Status. 1 = A decrementer event has occurred."),
            ((5,),     'FIS',  "Fixed Interval Timer (FIT) Interrupt Status. 1 = A FIT event has occurred."),
            ((6,31),   'RESV', "Reserved."),
        ]

    def _symbolic(self, field, val):
        if field == 'ENW':
            return "1 = Watchdog interrupt/reset enabled." if val else "0 = ENW will be set on next WDT event."
        if field == 'WIS':
            return "1 = A watchdog timer event has occurred." if val else "0 = No watchdog timer event has occurred."
        if field == 'WRS':
            return {
                0b00: "00 = No watchdog timer reset occurred.",
                0b01: "01 = Last WDT action was soft reset.",
                0b10: "10 = Last WDT action was hard reset.",
                0b11: "11 = Last WDT action halted the processor."
            }.get(val, '')
        if field == 'DIS':
            return "1 = A decrementer event has occurred." if val else "0 = No decrementer event has occurred."
        if field == 'FIS':
            return "1 = A FIT event has occurred." if val else "0 = No FIT event has occurred."
        return ""

    def print_table(self):
        print("Register: Timer Status Register (TSR)")
        print(f"Value    : 0x{self.value:08X} / {format(self.value, '032b')}\n")

        col_widths = {
            'Bits': 9,
            'Field': 6,
            'Hex': 6,
            'Binary': 10,
            'DescWrap': 60,
        }

        header = f"{'Bits':<{col_widths['Bits']}} {'Field':<{col_widths['Field']}} {'Description':<{col_widths['DescWrap']}} {'Hex':<{col_widths['Hex']}} {'Binary':<{col_widths['Binary']}} {'Interpretation'}"
        print(header)
        print("-" * len(header))

        for (start, *maybe_end), field, desc in self._fields():
            end = maybe_end[0] if maybe_end else start
            bit_range = f"{start}" if start == end else f"{start}:{end}"
            val = self._get_bits(start, end)
            width = end - start + 1
            val_bin = format(val, f"0{width}b")
            val_hex = f"{val:X}"
            interp = self._symbolic(field, val)

            # wrap long description
            desc_lines = textwrap.wrap(desc, width=col_widths['DescWrap'])
            for i, line in enumerate(desc_lines):
                if i == 0:
                    print(f"{bit_range:<{col_widths['Bits']}} {field:<{col_widths['Field']}} {line:<{col_widths['DescWrap']}} {val_hex:<{col_widths['Hex']}} {val_bin:<{col_widths['Binary']}} {interp}")
                else:
                    print(f"{'':<{col_widths['Bits']}} {'':<{col_widths['Field']}} {line:<{col_widths['DescWrap']}}")

import textwrap

class TCRRegister:
    def __init__(self, value):
        self.value = value & 0xFFFFFFFF

    def _get_bits(self, start, end=None):
        if end is None:
            end = start
        mask = (1 << (end - start + 1)) - 1
        return (self.value >> start) & mask

    def _fields(self):
        return [
            ((0, 1),   'WP',  "Watchdog Period. Selects which timer input drives the watchdog timer."),
            ((2, 3),   'WRC', "Watchdog Reset Control. Selects action when watchdog timeout occurs."),
            ((4,),     'WIE', "Watchdog Interrupt Enable. 1 = watchdog interrupt is enabled."),
            ((5,),     'DIE', "Decrementer Interrupt Enable. 1 = decrementer interrupt is enabled."),
            ((6, 7),   'FP',  "FIT Period. Selects which timer input drives the fixed interval timer."),
            ((8,),     'FIE', "FIT Interrupt Enable. 1 = FIT interrupt is enabled."),
            ((9,10),   'DS',  "Decrementer Select. Selects the event that decrements DEC."),
            ((11, 31), 'RESV',"Reserved."),
        ]

    def _symbolic(self, field, val):
        if field == 'WP':
            return {
                0b00: 'WP = 00 = WDT uses timer[0]',
                0b01: 'WP = 01 = WDT uses timer[1]',
                0b10: 'WP = 10 = WDT uses timer[2]',
                0b11: 'WP = 11 = WDT uses timer[3]',
            }.get(val, '')
        if field == 'WRC':
            return {
                0b00: 'WRC = 00 = No reset',
                0b01: 'WRC = 01 = Soft reset',
                0b10: 'WRC = 10 = Hard reset',
                0b11: 'WRC = 11 = Force-halt',
            }.get(val, '')
        if field == 'WIE':
            return "WIE = 1 = watchdog interrupt is enabled." if val else ""
        if field == 'DIE':
            return "DIE = 1 = decrementer interrupt is enabled." if val else ""
        if field == 'FP':
            return {
                0b00: 'FP = 00 = FIT uses timer[0]',
                0b01: 'FP = 01 = FIT uses timer[1]',
                0b10: 'FP = 10 = FIT uses timer[2]',
                0b11: 'FP = 11 = FIT uses timer[3]',
            }.get(val, '')
        if field == 'FIE':
            return "FIE = 1 = FIT interrupt is enabled." if val else ""
        if field == 'DS':
            return {
                0b00: 'DS = 00 = DEC decrements every cycle',
                0b01: 'DS = 01 = DEC decrements on timer[1]',
                0b10: 'DS = 10 = DEC decrements on dec_timer',
                0b11: 'DS = 11 = DEC decrements on timer[3]',
            }.get(val, '')
        return ""

    def print_table(self):
        print("Register: Timer Control Register (TCR)")
        print(f"Value    : 0x{self.value:08X} / {format(self.value, '032b')}\n")

        col_widths = {
            'Bits': 9,
            'Field': 6,
            'Hex': 6,
            'Binary': 10,
            'DescWrap': 60,
        }

        header = f"{'Bits':<{col_widths['Bits']}} {'Field':<{col_widths['Field']}} {'Description':<{col_widths['DescWrap']}} {'Hex':<{col_widths['Hex']}} {'Binary':<{col_widths['Binary']}} {'Interpretation'}"
        print(header)
        print("-" * len(header))

        for (start, *maybe_end), field, desc in self._fields():
            end = maybe_end[0] if maybe_end else start
            bit_range = f"{start}" if start == end else f"{start}:{end}"
            val = self._get_bits(start, end)
            width = end - start + 1
            val_bin = format(val, f"0{width}b")
            val_hex = f"{val:X}"
            interp = self._symbolic(field, val)

            desc_lines = textwrap.wrap(desc, width=col_widths['DescWrap'])
            for i, line in enumerate(desc_lines):
                if i == 0:
                    print(f"{bit_range:<{col_widths['Bits']}} {field:<{col_widths['Field']}} {line:<{col_widths['DescWrap']}} {val_hex:<{col_widths['Hex']}} {val_bin:<{col_widths['Binary']}} {interp}")
                else:
                    print(f"{'':<{col_widths['Bits']}} {'':<{col_widths['Field']}} {line:<{col_widths['DescWrap']}}")


import textwrap

class SRR0Register:
    def __init__(self, value):
        self.value = value & 0xFFFFFFFF

    def _fields(self):
        return [
            ((0, 31), 'PC', "Saved Program Counter. Address to resume execution after an exception."),
        ]

    def print_table(self):
        print("Register: Save/Restore Register 0 (SRR0)")
        print(f"Value    : 0x{self.value:08X} / {format(self.value, '032b')}\n")

        col_widths = {
            'Bits': 9,
            'Field': 6,
            'Hex': 8,
            'Binary': 34,
            'DescWrap': 60,
        }

        header = f"{'Bits':<{col_widths['Bits']}} {'Field':<{col_widths['Field']}} {'Description':<{col_widths['DescWrap']}} {'Hex':<{col_widths['Hex']}} {'Binary':<{col_widths['Binary']}}"
        print(header)
        print("-" * len(header))

        for (start, end), field, desc in self._fields():
            bit_range = f"{start}:{end}" if start != end else f"{start}"
            width = end - start + 1
            val = (self.value >> start) & ((1 << width) - 1)
            val_bin = format(val, f"0{width}b")
            val_hex = f"{val:0{col_widths['Hex'] - 2}X}"

            desc_lines = textwrap.wrap(desc, width=col_widths['DescWrap'])
            for i, line in enumerate(desc_lines):
                if i == 0:
                    print(f"{bit_range:<{col_widths['Bits']}} {field:<{col_widths['Field']}} {line:<{col_widths['DescWrap']}} 0x{val_hex:<{col_widths['Hex'] - 2}} {val_bin:<{col_widths['Binary']}}")
                else:
                    print(f"{'':<{col_widths['Bits']}} {'':<{col_widths['Field']}} {line:<{col_widths['DescWrap']}}")

import textwrap

class ISRRegister:
    def __init__(self, value):
        self.value = value & 0xFFFFFFFF

    def _get_bits(self, start, end=None):
        if end is None:
            end = start
        mask = (1 << (end - start + 1)) - 1
        return (self.value >> start) & mask

    def _fields(self):
        return [
            ((0, 15),  'RESV0', 'Reserved.'),
            ((16, 19), 'SRSMS', 'System Reset State Machine State. This field records the value of the PPE42 core state machine state at the time of the most recent system reset.'),
            ((20,),    'RESV1', 'Reserved.'),
            ((21,),    'EP',    'MSR[EE] Maskable Event Pending. 1 = An event maskable by MSR[EE] is pending.'),
            ((22, 23), 'RESV2', 'Reserved.'),
            ((24,),    'PTR',   'Program Interrupt from trap. 1 = Program interrupt caused by trap instruction.'),
            ((25,),    'ST',    'Data Interrupt Caused by a Store. 1 = Data interrupt caused by a store.'),
            ((26, 27), 'RESV3', 'Reserved.'),
            ((28,),    'MFE',   'Multiple Fault Error. 1 = Multiple imprecise store-type machine checks reported.'),
            ((29, 31), 'MCS',   'Machine Check Status:\n000 = Instruction machine check\n001 = Data machine check – load\n010 = Data machine check – precise store\n011 = Data machine check – imprecise store\n100 = Program interrupt, promoted\n101 = Instruction storage interrupt, promoted\n110 = Alignment interrupt, promoted\n111 = Data storage interrupt, promoted'),
        ]

    def _symbolic(self, field, val):
        if field == 'EP' and val:
            return '1 = An event maskable by MSR[EE] is pending.'
        if field == 'PTR' and val:
            return '1 = Program interrupt caused by trap instruction.'
        if field == 'ST' and val:
            return '1 = Data interrupt caused by a store.'
        if field == 'MFE' and val:
            return '1 = Multiple imprecise store-type machine checks reported.'
        if field == 'MCS':
            return {
                0b000: '000 = Instruction machine check',
                0b001: '001 = Data machine check – load',
                0b010: '010 = Data machine check – precise store',
                0b011: '011 = Data machine check – imprecise store',
                0b100: '100 = Program interrupt, promoted',
                0b101: '101 = Instruction storage interrupt, promoted',
                0b110: '110 = Alignment interrupt, promoted',
                0b111: '111 = Data storage interrupt, promoted',
            }.get(val, '')
        return ""

    def print_table(self):
        print("Register: Interrupt Status Register (ISR)")
        print(f"Value    : 0x{self.value:08X} / {format(self.value, '032b')}\n")

        col_widths = {
            'Bits': 9,
            'Field': 7,
            'Hex': 6,
            'Binary': 10,
            'DescWrap': 60,
        }

        header = f"{'Bits':<{col_widths['Bits']}} {'Field':<{col_widths['Field']}} {'Description':<{col_widths['DescWrap']}} {'Hex':<{col_widths['Hex']}} {'Binary':<{col_widths['Binary']}} {'Interpretation'}"
        print(header)
        print("-" * len(header))

        for (start, *maybe_end), field, desc in self._fields():
            end = maybe_end[0] if maybe_end else start
            bit_range = f"{start}" if start == end else f"{start}:{end}"
            val = self._get_bits(start, end)
            width = end - start + 1
            val_bin = format(val, f"0{width}b")
            val_hex = f"{val:X}"
            interp = self._symbolic(field, val)

            desc_lines = textwrap.wrap(desc, width=col_widths['DescWrap'])
            for i, line in enumerate(desc_lines):
                if i == 0:
                    print(f"{bit_range:<{col_widths['Bits']}} {field:<{col_widths['Field']}} {line:<{col_widths['DescWrap']}} {val_hex:<{col_widths['Hex']}} {val_bin:<{col_widths['Binary']}} {interp}")
                else:
                    print(f"{'':<{col_widths['Bits']}} {'':<{col_widths['Field']}} {line:<{col_widths['DescWrap']}}")


# Example usage
if __name__ == "__main__":
    reg_val = 0x8F123456  # Replace with your actual MSR register value
    msr = MSRRegister(reg_val)
    msr.print_table()

    reg_val = 0x00000003  # Example XCR value
    xcr = XCRRegister(reg_val)
    xcr.print_table()

    reg_val = 0x10400280  # Example XSR value
    xsr = XSRRegister(reg_val)
    xsr.print_table()

    reg_val = 0x00000007  # Example: SO=1, OV=1, CA=1
    xer = XERRegister(reg_val)
    xer.print_table()

    reg_val = 0x0000002F  # Example: All status bits set
    tsr = TSRRegister(reg_val)
    tsr.print_table()

    reg_val = 0x000003AB  # Example: various fields set
    tcr = TCRRegister(reg_val)
    tcr.print_table()

    srr0_val = 0x00101234
    srr0 = SRR0Register(srr0_val)
    srr0.print_table()

    isr_val = 0xB0000020  # EP, PTR, ST, MFE, MCS=101
    isr = ISRRegister(isr_val)
    isr.print_table()
