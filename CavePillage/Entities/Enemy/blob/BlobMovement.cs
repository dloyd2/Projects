using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlobMovement : MonoBehaviour
{
    [SerializeField] CharacterController2D controller;
    Transform player;               // Reference to the player's position.
    PlayerStats playerHealth;      // Reference to the player's health.
    BlobHealth enemyHealth;        // Reference to this enemy's health.
    public Animator anim;
    [SerializeField] float moveSpeed = 40f;
    float horizontalMove = 0f;
    float waitTime = 1f;
    float timer;
    
    public void UpdateMove()
    {
        timer = 0f;
    }
    
    void Awake ()
    {
        // Set up the references.
        timer = waitTime;
        player = GameObject.FindGameObjectWithTag ("Player").transform;
        playerHealth = player.GetComponent <PlayerStats> ();
        enemyHealth = GetComponent <BlobHealth> ();
    }
    
    
    void Update ()
    {
        timer += Time.deltaTime;
        // If the enemy and the player have health left...
        if(timer >= waitTime && enemyHealth.GetHealth() > 0 && !playerHealth.GetIsDead())
        {
            horizontalMove = player.position.x > gameObject.transform.position.x ? moveSpeed : -1*moveSpeed;
            controller.Move(horizontalMove * Time.fixedDeltaTime, false, true);
            //anim.SetFloat("Speed", horizontalMove); get animations working(about to jump/land events?)
        }
        // Otherwise...
        else
        {
            // ... disable the nav mesh agent.
            
        }
        
        horizontalMove = 0;
    }
}
