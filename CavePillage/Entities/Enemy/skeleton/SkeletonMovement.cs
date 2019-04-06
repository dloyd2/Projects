using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SkeletonMovement : MonoBehaviour
{
    [SerializeField] CharacterController2D controller;
    Transform player;               // Reference to the player's position.
    PlayerStats playerHealth;      // Reference to the player's health.
    Skeleton enemy;        // Reference to this enemy's health.
    public Animator anim;
    [SerializeField] float moveSpeed = 20f;
    float horizontalMove = 0f;
    bool jump = false;
    bool canMove = true;
    bool attacking = false;
    bool moving = true;
    
    float diffInPosX = 0;
    float diffInPosY = 0;
    float playerPosX, thisPosX;
    
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
        player = GameObject.FindGameObjectWithTag ("Player").transform;
        playerHealth = player.GetComponent <PlayerStats> ();
        enemy = GetComponent <Skeleton> ();
    }
    
    
    void Update ()
    {
        moving = true;
        playerPosX = player.position.x;
        thisPosX = gameObject.transform.position.x;
        diffInPosX = Mathf.Abs(player.position.x-gameObject.transform.position.x);
        if(!enemy.GetIsDead() && !playerHealth.GetIsDead())
        {
            if(canMove)
            {
                horizontalMove = moveSpeed;
                if(diffInPosX >= 4 && diffInPosX < 5)
                {
                    horizontalMove = 0.00001f;
                    moving = false;
                }
                if(diffInPosX < 4)
                {
                    horizontalMove *= -1;
                }
                if(playerPosX < thisPosX)
                {
                    horizontalMove *= -1;
                }
            }
            else
            {
                horizontalMove = 0f;
            }
            anim.SetBool("Attacking_range", attacking);
            anim.SetFloat("Speed", Mathf.Abs(horizontalMove));
            controller.Move(horizontalMove * Time.fixedDeltaTime, false, jump);
        }
        
        horizontalMove = 0;
    }
}
