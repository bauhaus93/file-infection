pipeline {
	agent any
	stages {
		stage("Build") {
			steps {
				sh 'make target-native-setup'
				sh 'make target-native-build'
			}
		}
		stage("Test") {
			steps {
				sh 'make target-native-test'
			}
		}
	}

}
