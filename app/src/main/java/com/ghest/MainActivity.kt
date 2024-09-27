package com.ghest

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import com.amplifyframework.ui.authenticator.ui.Authenticator
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import com.amplifyframework.api.graphql.model.ModelMutation
import com.amplifyframework.api.graphql.model.ModelQuery
import com.amplifyframework.api.graphql.model.ModelSubscription
import com.amplifyframework.core.Amplify
import com.amplifyframework.datastore.generated.model.Todo
import com.ghest.ui.theme.GhestTheme

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
                    Authenticator {
                        Column {
                            /*Text(
                                text = "Hello ${state.user.userId}!",
                            )*/ // Returns userID, required functionally is to return email.
                            Text(
                                text = "Welcome to the Ghest Companion App!\n"
                            )

                            TodoList()
                            Button(onClick = {
                                val todo = Todo.builder()
                                    .content("Test Data")
                                            /*("USER {\n" +
                                            "Name: Adam Frederick\n" +
                                            "Email:afred574@live.kutztown.edu\n" +
                                            "DOB: \n" +
                                            "Bracelet ID: None registered\n" +
                                            "Registration Date: \n" +
                                            "}\n" +
                                            "\n" +
                                            "BRACELET {\n" +
                                            "Name: \n" +
                                            "Bracelet ID: \n" +
                                            "MAC Address: \n" +
                                            "Registration Date: \n" +
                                            "IsSynced: False\n" +
                                            "}\n")*/
                                    .build()

                                Amplify.API.mutate(
                                    ModelMutation.create(todo),
                                    { Log.i("MyAmplifyApp", "Added Todo with id: ${it.data.id}") },
                                    { Log.e("MyAmplifyApp", "Create failed", it) }
                                )
                            }) {
                                Text(text = "Display User Data")
                            }

                            Button(onClick = {
                                val todo = Todo.builder()
                                    .content("\nTo be completed in Sprint 2.\n")
                                    .build()

                                Amplify.API.mutate(
                                    ModelMutation.create(todo),
                                    { Log.i("MyAmplifyApp", "Added Todo with id: ${it.data.id}")},
                                    { Log.e("MyAmplifyApp", "Create failed", it)},
                                )
                            }) {
                                Text(text = "Sync Bracelet")
                            }
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

@Composable
fun TodoList() {
    var todoList by remember { mutableStateOf(emptyList<Todo>()) }

    LaunchedEffect(Unit) {
        // API request to list all Todos
        Amplify.API.query(
            ModelQuery.list(Todo::class.java),
            { todoList = it.data.items.toList() },
            { Log.e("AWS Amplify", "Failed to query.", it) }
        )
    }

    Amplify.API.subscribe(
        ModelSubscription.onCreate(Todo::class.java),
        { Log.i("ApiQuickStart", "Subscription established") },
        {
            Log.i("ApiQuickStart", "Todo create subscription received: ${it.data}")
            todoList = todoList + it.data
        },
        { Log.e("ApiQuickStart", "Subscription failed", it) },
        { Log.i("ApiQuickStart", "Subscription completed") }
    )
    LazyColumn {
        items(todoList) { todo ->
            Row {
                Text(text = todo.content)
            }
        }
    }
}
