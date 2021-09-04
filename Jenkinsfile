pipeline {
	agent any
	stages {
		stage("Build") {
			steps {
				make target-native-setup
				make target-native-build
			}
		}
		stage("Test") {
			steps {
				make target-native-test
			}
		}
	}

}
