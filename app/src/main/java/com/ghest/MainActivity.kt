package com.ghest

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.ghest.ui.theme.GhestTheme
import com.amplifyframework.ui.authenticator.ui.Authenticator
import androidx.compose.foundation.layout.Column
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import com.amplifyframework.core.Amplify


class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            GhestTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                )

                {
                    Authenticator { state ->
                        Column {
                            Text(
                                text = "Hello ${state.user.username}!",
                            )
                            Button(onClick = {
                                Amplify.Auth.signOut {  }
                            }) {
                                Text(text = "Sign Out")
                            }
                        }
                    }
                }

            }
        }
    }
}