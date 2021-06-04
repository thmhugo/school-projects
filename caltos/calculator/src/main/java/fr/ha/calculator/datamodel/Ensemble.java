package fr.ha.calculator.datamodel;

import java.util.HashSet ;

public class Ensemble extends Element {
    private final Value<HashSet <Element>> value;

    Ensemble(Value<HashSet <Element>> value) {
        this.value = value;
    }

    @Override
    public HashSet<Element> getValue() {
        return value.getValue();
    }

    @Override
    public String toString() {
        StringBuilder strb = new StringBuilder();
        value.getValue().forEach(e -> strb.append(e+";"));
        String str = strb.toString();
        if (str.length()>1) str = str.substring(0, str.length()-1);
        str += "}";
        return "{"+str;
    }

    @Override
    public boolean equals(Object e) {
        if ((e instanceof Ensemble)){
            Element[] valToArray = (Element[])value.getValue().toArray();
            HashSet<Element> sourceSet =  ((Ensemble)e).getValue();
            
            if (sourceSet.size() != valToArray.length) {
                return false;
            }

            for (Element elt: valToArray) {
                if (!sourceSet.contains(elt)) {
                    return false;
                }
            }
            
            return true;
        } 
        else {
            return false;
        }
    }
}
