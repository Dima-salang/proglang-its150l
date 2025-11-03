module com.grp3project {
    requires javafx.controls;
    requires javafx.fxml;

    opens com.grp3project to javafx.fxml;
    exports com.grp3project;
}
