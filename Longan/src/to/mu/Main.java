package to.mu;

import java.lang.reflect.Method;

public class Main {

    @UseCase(id = 45)
    public boolean validataePassword(String password) {
        return (password.matches("\\w*\\d\\w*"));
    }

    public static void main(String[] args) {
        for (Method m : Main.class.getDeclaredMethods()){
            UseCase uc = m.getAnnotation(UseCase.class);
            if (uc != null) {
                System.out.print("found [" + uc.description() + "," + uc.id());
            }
        }
    }
}
