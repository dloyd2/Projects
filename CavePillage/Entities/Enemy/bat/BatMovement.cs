using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BatMovement : MonoBehaviour
{
    [SerializeField] BatController controller;
    Transform player;               // Reference to the player's position.
    PlayerStats playerHealth;       // Reference to the player's health.
    Bat enemy;        // Reference to this enemy's health.
    public Animator anim;
    [SerializeField] float moveSpeed = 20f;
    float horizontalMove = 0f;
    bool jump = true;
    bool canMove = true;
    bool attacking = false;
    bool moving = true;
    
    float diffInPosX = 0;
    float diffInPosY = 0;
    float playerPosX, thisPosX;
    float jumpTimer;
    float jumpTimerMax = 1f;
    
    public bool GetMoving()
    {
        return moving;
    }
    public float GetSpeed()
    {
        return horizontalMove;
    }
    public void SetMove(bool newState)
    {
        canMove = newState;
    }
    public void SetAttacking(bool newState)
    {
        attacking = newState;
    }
    void Awake ()
    {
        // Set up the references.
        jumpTimer = jumpTimerMax;
        player = GameObject.FindGameObjectWithTag ("Player").transform;
        playerHealth = player.GetComponent <PlayerStats> ();
        enemy = GetComponent <Bat> ();
    }
    
    
    void Update ()
    {
        jumpTimer -= Time.deltaTime;
        moving = true;
        playerPosX = player.position.x;
        thisPosX = gameObject.transform.position.x;
        diffInPosX = Mathf.Abs(player.position.x-gameObject.transform.position.x);
        diffInPosY = (player.position.y-gameObject.transform.position.y);
        if(!enemy.GetIsDead() && !playerHealth.GetIsDead())
        {
            if(canMove)
            {
                horizontalMove = moveSpeed;
                if(diffInPosX >= 0.0f && diffInPosX < 0.5f)
                {
                    horizontalMove = 0.00001f;
                    moving = false;
                }
               
                if(playerPosX < thisPosX)
                {
                    horizontalMove *= -1;
                }
                if(diffInPosY >= -1 && jumpTimer <= 0){jump = true; jumpTimer = jumpTimerMax;}
            }
            else
            {
                horizontalMove = 0f;
            }
            anim.SetFloat("Speed", Mathf.Abs(horizontalMove));
            controller.Move(horizontalMove * Time.fixedDeltaTime, false, jump);
        }
        
        horizontalMove = 0;
        jump = false;
    }
}
