import("stdfaust.lib");

frequency = nentry("freq",440,20,20000,0.01) : si.smoo;
level = nentry("gain",1,0,1,0.01) : si.smoo;
gate = button("trigger");
env = en.ar(0.07, 0.5, gate) * level;

hit_sound = os.osc(frequency) + os.osc(frequency/1.5)+ os.osc(frequency/2);

process = vgroup("saw", hit_sound * env : si.smoo);

